#include "PktDef.h"
#include <cstring>
#include <bitset>

//define deafult constructor
PktDef::PktDef() {
	//set all header info to 0
	cmdPacket.header.PktCount = 0;
	cmdPacket.header.cmdFlags.drive = 0;
	cmdPacket.header.cmdFlags.ack = 0;
	cmdPacket.header.cmdFlags.sleep = 0;
	cmdPacket.header.cmdFlags.status = 0;
	cmdPacket.header.cmdFlags.padding = 0;
	cmdPacket.length = 0;
	cmdPacket.Data = nullptr;
	cmdPacket.CRC = 0;
	RawBuffer = nullptr;
}

//overloaded PktDef constructor
PktDef::PktDef(char* rawData) {
	//copy header
	memcpy(&cmdPacket.header, rawData, HEADERSIZE);

	//get length (now 1 byte)
	cmdPacket.length = rawData[HEADERSIZE];

	//allocate and copy data if length > 0
	if (cmdPacket.length > 0) {
		cmdPacket.Data = new char[cmdPacket.length];
		memcpy(cmdPacket.Data, rawData + HEADERSIZE + 1, cmdPacket.length);
	}
	else {
		cmdPacket.Data = nullptr;
	}

	//crc from the end of the packet
	cmdPacket.CRC = rawData[HEADERSIZE + 1 + cmdPacket.length];
	RawBuffer = nullptr;
}

//destructor
PktDef::~PktDef() {
	if (cmdPacket.Data != nullptr) {
		delete[] cmdPacket.Data;
	}
	if (RawBuffer != nullptr) {
		delete[] RawBuffer;
	}
}

//setCMDType function
void PktDef::setCMD(CMDType cmd) {
	//reset flags
	cmdPacket.header.cmdFlags.drive = 0;
	cmdPacket.header.cmdFlags.status = 0;
	cmdPacket.header.cmdFlags.sleep = 0;

	switch (cmd) {
	case CMDType::DRIVE:
		cmdPacket.header.cmdFlags.drive = 1;
		break;
	case CMDType::SLEEP:
		cmdPacket.header.cmdFlags.sleep = 1;
		break;
	case CMDType::RESPONSE:
		cmdPacket.header.cmdFlags.status = 1;
		break;
	}
}

//setBodyData function
void PktDef::setBodyData(char* data, int size) {
	if (cmdPacket.Data != nullptr) {
		delete[] cmdPacket.Data;
	}
	cmdPacket.Data = new char[size];
	memcpy(cmdPacket.Data, data, size);
	cmdPacket.length = size;
}

//setPktcount
void PktDef::setPktCount(int size) {
	cmdPacket.header.PktCount = size;
}

CMDType PktDef::getCMD() {
	if (cmdPacket.header.cmdFlags.drive) return CMDType::DRIVE;
	if (cmdPacket.header.cmdFlags.sleep) return CMDType::SLEEP;
	if (cmdPacket.header.cmdFlags.status) return CMDType::RESPONSE;
	return CMDType::DRIVE; // default
}

//check ack status
bool PktDef::getAck() {
	return cmdPacket.header.cmdFlags.ack;
}

//getlength
int PktDef::getLength() {
	return cmdPacket.length;
}

//getbody
char* PktDef::getBodyData() {
	return cmdPacket.Data;
}

//get Pkt count
int PktDef::getPktCount() {
	return cmdPacket.header.PktCount;
}

bool PktDef::checkCRC(char* buffer, int size) {
	char calculatedCRC = 0;
	for (int i = 0; i < size - 1; i++) {
		std::bitset<8> bits(buffer[i]);
		calculatedCRC += bits.count();
	}
	return calculatedCRC == buffer[size - 1];
}

void PktDef::calcCRC() {
	char calculatedCRC = 0;

	//calculate crc for header
	std::bitset<8> headerBits(cmdPacket.header.PktCount);
	calculatedCRC += headerBits.count();
	std::bitset<8> flagsBits(*reinterpret_cast<uint8_t*>(&cmdPacket.header.cmdFlags));
	calculatedCRC += flagsBits.count();

	//add length to crc
	std::bitset<8> lengthBits(cmdPacket.length);
	calculatedCRC += lengthBits.count();

	//calculate crc for data if it exists
	if (cmdPacket.Data != nullptr) {
		for (int i = 0; i < cmdPacket.length; i++) {
			std::bitset<8> dataBits(cmdPacket.Data[i]);
			calculatedCRC += dataBits.count();
		}
	}

	cmdPacket.CRC = calculatedCRC;
}

char* PktDef::genPacket() {
	if (RawBuffer != nullptr) {
		delete[] RawBuffer;
	}

	//calculate total size
	int totalSize = HEADERSIZE + 1 + cmdPacket.length + 1; // Header + Length + Data + CRC

	//allocate buffer
	RawBuffer = new char[totalSize];

	//copy header
	memcpy(RawBuffer, &cmdPacket.header, HEADERSIZE);

	//copy length
	RawBuffer[HEADERSIZE] = cmdPacket.length;

	//copy data if it exists
	if (cmdPacket.Data != nullptr) {
		memcpy(RawBuffer + HEADERSIZE + 1, cmdPacket.Data, cmdPacket.length);
	}

	//calculate and copy crc
	calcCRC();
	RawBuffer[totalSize - 1] = cmdPacket.CRC;

	return RawBuffer;
}


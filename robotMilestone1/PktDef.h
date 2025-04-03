#pragma once
#include <cstdint>

//enumerated CMDType with specified command types
enum class CMDType {
	DRIVE,
	SLEEP,
	RESPONSE
};

//constants
const uint8_t FORWARD = 1;
const uint8_t BACKWARD = 2;
const uint8_t RIGHT = 3;
const uint8_t LEFT = 4;
const int HEADERSIZE = 3;		//headersize = PktCount(2 bytes) + 1 byte flag = 2

//header
struct Header {
	uint16_t PktCount;
	struct {
		uint8_t drive : 1;
		uint8_t status : 1;
		uint8_t sleep : 1;
		uint8_t ack : 1;
		uint8_t padding : 4;
		uint8_t length : 2;
	} cmdFlags;
};

// drivebody struct
struct driveBody {
	uint8_t direction;
	uint8_t duration;
	uint8_t speed;
};


class PktDef {
private:
	struct CmdPacket {
		Header header;
		char* Data;
		char CRC;
	}cmdPacket;

	char* RawBuffer;

public:

	PktDef();
	PktDef(char* rawData);
	void setCMD(CMDType cmd);
	void setBodyData(char* data, int size);
	void setPktCount(int);
	CMDType getCMD();
	bool getAck();
	int getLength();
	char* getBodyData();
	int getPktCount();
	bool checkCRC(char* buffer, int size);
	void calcCRC();				//counting number of 1s
	char* genPacket();
	~PktDef();

};
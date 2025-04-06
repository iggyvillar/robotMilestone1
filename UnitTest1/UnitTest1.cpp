#include "pch.h"
#include "CppUnitTest.h"
#include "../robotMilestone1/PktDef.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace PktDefTests
{
    TEST_CLASS(PktDefTests)
    {
    public:
        //test default constructor, and that all values initialized to 0/null
        TEST_METHOD(defaultConstructorTest)
        {
            PktDef packet;
            Assert::AreEqual((int)0, (int)packet.getPktCount());
            Assert::IsFalse(packet.getAck());
            Assert::AreEqual((int)0, packet.getLength());
            Assert::IsNull(packet.getBodyData());
        }

        //test packet generation
        TEST_METHOD(genPacketTest)
        {
            PktDef packet;
            packet.setPktCount(1);
            packet.setCMD(CMDType::DRIVE);
            char testData[] = "lebron";
            packet.setBodyData(testData, sizeof(testData));

            char* rawPacket = packet.genPacket();
            Assert::IsNotNull(rawPacket);

            // Verify header
            uint16_t pktCount = *reinterpret_cast<uint16_t*>(rawPacket);
            Assert::AreEqual((int)1, (int)pktCount);

            // Verify command flag
            uint8_t flags = rawPacket[2];
            Assert::IsTrue(flags & 0x01); // should be drive
        }

        //test packet gen with empty body
        TEST_METHOD(emptyPacketTest)
        {
            PktDef packet;
            packet.setPktCount(1);
            packet.setCMD(CMDType::DRIVE);

            char* rawPacket = packet.genPacket();
            Assert::IsNotNull(rawPacket);
            Assert::AreEqual((int)0, packet.getLength());
            Assert::IsNull(packet.getBodyData());
        }

        //test body data is stored correctly
        TEST_METHOD(setBodyDataTest)
        {
            PktDef packet;
            char testData[] = "lebron";
            packet.setBodyData(testData, sizeof(testData));
            Assert::AreEqual((int)sizeof(testData), packet.getLength());
            Assert::IsNotNull(packet.getBodyData());
        }

        //test command types are all able to be set properly
        TEST_METHOD(setCMDTest)
        {
            PktDef packet;

            // test DRIVE 
            packet.setCMD(CMDType::DRIVE);
            Assert::IsTrue(packet.getCMD() == CMDType::DRIVE);

            // test SLEEP 
            packet.setCMD(CMDType::SLEEP);
            Assert::IsTrue(packet.getCMD() == CMDType::SLEEP);

            // test RESPONSE 
            packet.setCMD(CMDType::RESPONSE);
            Assert::IsTrue(packet.getCMD() == CMDType::RESPONSE);
        }

        //test packet count set 
        TEST_METHOD(setPktCountTest)
        {
            PktDef packet;
            packet.setPktCount(12);
            Assert::AreEqual((int)12, (int)packet.getPktCount());
        }

        //test CRC calculation is correct
        TEST_METHOD(crcTest)
        {
            PktDef packet;
            char testData[] = "lebron";
            packet.setBodyData(testData, sizeof(testData));
            packet.calcCRC();

            char* rawPacket = packet.genPacket();
            Assert::IsTrue(packet.checkCRC(rawPacket, HEADERSIZE + 1 + sizeof(testData) + 1));
        }

        //test invalid CRC 
        TEST_METHOD(invalidCRCTest)
        {
            PktDef packet;
            char testData[] = "lebron";
            packet.setBodyData(testData, sizeof(testData));
            packet.calcCRC();

            char* rawPacket = packet.genPacket();
            
            rawPacket[HEADERSIZE + 1 + sizeof(testData)] = 0xFF;
            Assert::IsFalse(packet.checkCRC(rawPacket, HEADERSIZE + 1 + sizeof(testData) + 1));
        }

        //test directions properly set
        TEST_METHOD(directionConstantsTest)
        {
            Assert::AreEqual((int)1, (int)FORWARD);
            Assert::AreEqual((int)2, (int)BACKWARD);
            Assert::AreEqual((int)3, (int)RIGHT);
            Assert::AreEqual((int)4, (int)LEFT);
        }

        //test telemetry data handling
        TEST_METHOD(telemetryTest)
        {
            PktDef packet;
            packet.setCMD(CMDType::RESPONSE);
            
            telemetry telemetryData;
            telemetryData.LastPktCounter = 123;
            telemetryData.CurrentGrade = 45;
            telemetryData.HitCount = 5;
            telemetryData.LastCmd = 1;
            telemetryData.LastCmdValue = 100;
            telemetryData.LastCmdSpeed = 50;
            
            packet.setBodyData(reinterpret_cast<char*>(&telemetryData), sizeof(telemetry));
            Assert::AreEqual((int)sizeof(telemetry), packet.getLength());
            
            char* rawPacket = packet.genPacket();
            Assert::IsNotNull(rawPacket);
            Assert::IsTrue(packet.checkCRC(rawPacket, HEADERSIZE + 1 + sizeof(telemetry) + 1));
        }
    };
}
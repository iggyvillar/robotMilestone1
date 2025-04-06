using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
#include "pch.h"
#include "CppUnitTest.h"
#include "../robotMilestone1/PktDef.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace PktDefTests
{
    TEST_CLASS(PktDefTests)
    {
    public:
        TEST_METHOD(DefaultConstructorTest)
        {
            PktDef packet;
            Assert::AreEqual(0, packet.getPktCount());
            Assert::IsFalse(packet.getAck());
            Assert::AreEqual(0, packet.getLength());
            Assert::IsNull(packet.getBodyData());
        }

        TEST_METHOD(SetCmdTest)
        {
            PktDef packet;

            // Test DRIVE command
            packet.setCMD(CMDType::DRIVE);
            Assert::IsTrue(packet.getCMD() == CMDType::DRIVE);

            // Test SLEEP command
            packet.setCMD(CMDType::SLEEP);
            Assert::IsTrue(packet.getCMD() == CMDType::SLEEP);

            // Test RESPONSE command
            packet.setCMD(CMDType::RESPONSE);
            Assert::IsTrue(packet.getCMD() == CMDType::RESPONSE);
        }

        TEST_METHOD(SetPktCountTest)
        {
            PktDef packet;
            packet.setPktCount(123);
            Assert::AreEqual(123, packet.getPktCount());
        }

        TEST_METHOD(SetBodyDataTest)
        {
            PktDef packet;
            char testData[] = "TestData";
            packet.setBodyData(testData, sizeof(testData));
            Assert::AreEqual(sizeof(testData), packet.getLength());
            Assert::IsNotNull(packet.getBodyData());
        }

        TEST_METHOD(CRCTest)
        {
            PktDef packet;
            char testData[] = "TestData";
            packet.setBodyData(testData, sizeof(testData));
            packet.calcCRC();

            char* rawPacket = packet.GenPacket();
            Assert::IsTrue(packet.checkCRC(rawPacket, packet.getLength() + HEADERSIZE + 2 + 1));
        }

        TEST_METHOD(GenPacketTest)
        {
            PktDef packet;
            packet.setPktCount(1);
            packet.setCMD(CMDType::DRIVE);
            char testData[] = "TestData";
            packet.setBodyData(testData, sizeof(testData));

            char* rawPacket = packet.GenPacket();
            Assert::IsNotNull(rawPacket);

            // Verify header
            uint16_t pktCount = *reinterpret_cast<uint16_t*>(rawPacket);
            Assert::AreEqual(1, (int)pktCount);

            // Verify command flag
            uint8_t flags = rawPacket[2];
            Assert::IsTrue(flags & 0x01); // Drive flag should be set
        }
    };
}
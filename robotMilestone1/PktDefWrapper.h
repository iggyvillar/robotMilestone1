#pragma once

#include "PktDef.h"

using namespace System;

namespace PktDefWrapper {
    public ref class PktDefManaged
    {
    private:
        PktDef* nativePktDef;

    public:
        PktDefManaged()
        {
            nativePktDef = new PktDef();
        }

        ~PktDefManaged()
        {
            delete nativePktDef;
        }

        void SetCmd(int cmdType)
        {
            nativePktDef->setCMD(static_cast<CMDType>(cmdType));
        }

        void SetBodyData(array<Byte>^ data)
        {
            pin_ptr<Byte> pinnedData = &data[0];
            nativePktDef->setBodyData(reinterpret_cast<char*>(pinnedData), data->Length);
        }

        void SetPktCount(int count)
        {
            nativePktDef->setPktCount(count);
        }

        int GetCmd()
        {
            return static_cast<int>(nativePktDef->getCMD());
        }

        bool GetAck()
        {
            return nativePktDef->getAck();
        }

        int GetLength()
        {
            return nativePktDef->getLength();
        }

        array<Byte>^ GetBodyData()
        {
            char* data = nativePktDef->getBodyData();
            if (data == nullptr)
                return nullptr;

            int length = nativePktDef->getLength();
            array<Byte>^ managedData = gcnew array<Byte>(length);
            pin_ptr<Byte> pinnedData = &managedData[0];
            memcpy(pinnedData, data, length);
            return managedData;
        }

        int GetPktCount()
        {
            return nativePktDef->getPktCount();
        }

        bool CheckCRC(array<Byte>^ buffer)
        {
            pin_ptr<Byte> pinnedBuffer = &buffer[0];
            return nativePktDef->checkCRC(reinterpret_cast<char*>(pinnedBuffer), buffer->Length);
        }

        void CalcCRC()
        {
            nativePktDef->calcCRC();
        }

        array<Byte>^ GenPacket()
        {
            char* rawPacket = nativePktDef->GenPacket();
            int length = nativePktDef->getLength() + HEADERSIZE + 2 + 1;
            array<Byte>^ managedPacket = gcnew array<Byte>(length);
            pin_ptr<Byte> pinnedPacket = &managedPacket[0];
            memcpy(pinnedPacket, rawPacket, length);
            return managedPacket;
        }
    };
} 
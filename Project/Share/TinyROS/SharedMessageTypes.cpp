#include "SharedMessageTypes.h"
#include "LibWrapper.h"

namespace RoboTax
{

#pragma region 地图消息
    TinyROS::TypeIDHash MapMessage::TypeHash = TinyROS::GetSHA("MapMessage", sizeof("MapMessage"));

    TinyROS::TypeIDHash MapMessage::GetTypeID()
    {
        return MapMessage::TypeHash;
    }

    std::string MapMessage::Serialize()
    {
        unsigned char* pBuffer;
        constexpr int bytesPerInt = sizeof(int) / sizeof(char);
        int len = 2 * bytesPerInt + this->Height * this->Width;
        pBuffer = new unsigned char[len];
        int* pHead = reinterpret_cast<int*>(pBuffer);
        pHead[0] = this->Width;
        pHead[1] = this->Height;
        std::copy(this->pData, this->pData + this->Width * this->Height, pBuffer + 2 * bytesPerInt);
        std::string s(reinterpret_cast<char*>(pBuffer), len);
        delete[] pBuffer;
        return s;
    }

    void MapMessage::Deserialize(std::string& str)
    {
        delete[] this->pData;
        constexpr int bytesPerInt = sizeof(int) / sizeof(char);
        const unsigned char* pBuffer = reinterpret_cast<const unsigned char*>(str.c_str());
        const int* pHead = reinterpret_cast<const int*>(pBuffer);
        this->Width = pHead[0];
        this->Height = pHead[1];
        this->pData = new unsigned char[this->Height * this->Width];
        std::copy(pBuffer + 2 * bytesPerInt, pBuffer + 2 * bytesPerInt + this->Height * this->Width, this->pData);
    }

    TinyROS::Message* MapMessage::NewDeserialize(std::string& str)
    {
        MapMessage* pMsg = new MapMessage();
        pMsg->Deserialize(str);
        return pMsg;
    }

    MapMessage::~MapMessage()
    {
        delete[] this->pData;
        this->pData = nullptr;
    }

    MapMessage::MapMessage()
    {
        this->Height = 0;
        this->Width = 0;
        this->pData = nullptr;
    }

    MapMessage::MapMessage(int width, int height)
    {
        this->Width = width;
        this->Height = height;
        this->pData = new unsigned char[width * height];
        std::fill(this->pData, this->pData + width * height, 0);
    }

    MapMessage::MapMessage(int width, int height, unsigned char* pData)
    {
        this->Width = width;
        this->Height = height;
        this->pData = new unsigned char[width * height];
        std::copy(pData, pData + width * height, this->pData);
    }

    MapMessage::MapMessage(const MapMessage& other)
    {
        this->Width = other.Width;
        this->Height = other.Height;
        this->pData = new unsigned char[other.Height + other.Width];
        std::copy(other.pData, other.pData + other.Width * other.Height, this->pData);
    }

    MapMessage::MapMessage(MapMessage&& other)
    {
        this->Width = other.Width;
        this->Height = other.Height;
        this->pData = other.pData;
        other.pData = nullptr;
    }

    unsigned char MapMessage::At(int row, int col)
    {
        // 需要测试一下
        int index = row * this->Width + col;
        if (index < 0 || index >= this->Height * this->Width)
        {
            throw std::out_of_range(std::to_string(index));
        }
        return this->pData[index];
    }

    unsigned char& MapMessage::RefAt(int col, int row)
    {
        int index = row * this->Width + col;
        if (index < 0 || index >= this->Height * this->Width)
        {
            throw std::out_of_range(std::to_string(index));
        }
        return this->pData[index];
    }

    int MapMessage::GetWidth()
    {
        return this->Width;
    }

    int MapMessage::GetHeight()
    {
        return this->Height;
    }

    void MapMessage::CopyTo(unsigned char* pBuffer)
    {
        if (this->pData == nullptr)
        {
            return;
        }
        std::copy(this->pData, this->pData + this->Height * this->Width, pBuffer);
    }
#pragma endregion



}
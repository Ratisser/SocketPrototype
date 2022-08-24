#include "ChattingPacket2.h"

#include <iostream>

ChattingPacket2::ChattingPacket2()
{

}

ChattingPacket2::~ChattingPacket2()
{

}

void ChattingPacket2::initPacketID()
{
    SetPacketID(ePacketID::Chat2);
}

void ChattingPacket2::userSerialize()
{
    serializer_ << text_;
}

void ChattingPacket2::userDeserialize()
{
    serializer_ >> text_;
}

GameEnginePacketBase* ChattingPacket2::getUserObject()
{
    return new ChattingPacket2;
}

void ChattingPacket2::execute(bool _bServer, GameEngineSocketInterface* _network)
{
    std::cout << text_ << "이 메시지는 채팅패킷2 입니다." << std::endl;

    if (_bServer == true)
    {
        _network->Send(this);
    }
}

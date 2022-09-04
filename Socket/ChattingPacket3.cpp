#include "ChattingPacket3.h"
#include <iostream>

ChattingPacket3::ChattingPacket3()
{

}

ChattingPacket3::~ChattingPacket3()
{

}

void ChattingPacket3::initPacketID()
{
    SetPacketID(ePacketID::Chat3);
}

void ChattingPacket3::userSerialize()
{
    serializer_ << text_;
}

void ChattingPacket3::userDeserialize()
{
    serializer_ >> text_;
}

GameEnginePacketBase* ChattingPacket3::getUserObject()
{
    return new ChattingPacket3;
}

void ChattingPacket3::execute(SOCKET _socketSender, GameEngineSocketInterface* _network, bool _bServer)
{
    std::cout << text_ << "이 패킷은 패킷3 입니다." << std::endl;

    if (_bServer)
    {
        _network->Send(_socketSender, this);
        _network->Send(this);
    }
}

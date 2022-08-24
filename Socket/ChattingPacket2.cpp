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
    std::cout << text_ << "�� �޽����� ä����Ŷ2 �Դϴ�." << std::endl;

    if (_bServer == true)
    {
        _network->Send(this);
    }
}

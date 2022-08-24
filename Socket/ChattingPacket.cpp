#include "ChattingPacket.h"
#include <iostream>

ChattingPacket::ChattingPacket()
{

}

ChattingPacket::~ChattingPacket()
{

}

void ChattingPacket::userSerialize()
{
	serializer_ << text_;
}

void ChattingPacket::userDeserialize()
{
	serializer_ >> text_;
}

void ChattingPacket::initPacketID()
{
	packetID_ = ePacketID::Chat;
}

GameEnginePacketBase* ChattingPacket::getUserObject()
{
	return new ChattingPacket;
}

void ChattingPacket::execute(bool _bServer, GameEngineSocketInterface* _network)
{
	std::cout << text_ << std::endl;

	if (_bServer)
	{
		_network->Send(this);
	}
}

void ChattingPacket::SetText(std::string& _text)
{
	if (_text.size() > MAX_TEXT_SIZE)
	{
		return;
	}

	text_ = std::move(_text);
}
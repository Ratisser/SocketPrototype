#include "ChattingPacket.h"

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

void ChattingPacket::SetText(std::string& _text)
{
	if (_text.size() > MAX_TEXT_SIZE)
	{
		return;
	}

	text_ = std::move(_text);
}

void ChattingPacket::setPacketID()
{
	packetID_ = 1;
}

#include "GameEnginePacketBase.h"

GameEnginePacketBase::GameEnginePacketBase()
	: packetID_(-1)
	, packetSize_(0)
{

}

GameEnginePacketBase::GameEnginePacketBase(char* _data, int _size)
	: packetID_(-1)
	, packetSize_(_size)
	, serializer_(_data, _size)
{
	
}

GameEnginePacketBase::~GameEnginePacketBase()
{

}

void GameEnginePacketBase::Serialize()
{
	serializer_ << packetID_;
	serializer_ << serializer_.GetOffSet();
	userSerialize();

	unsigned char* data = serializer_.GetDataPtr();
	unsigned int size = serializer_.GetOffSet();
	memcpy(data + 4, &size, sizeof(unsigned int));
}

void GameEnginePacketBase::Deserialize()
{
	serializer_ >> packetID_;
	serializer_ >> packetSize_;
	userDeserialize();
}

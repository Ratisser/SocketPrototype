#pragma once

#include "GameEngineSerializer.h"

class GameEnginePacketBase
{
public:
	GameEnginePacketBase();
	GameEnginePacketBase(char* _data, int _size);
	virtual ~GameEnginePacketBase();
	GameEnginePacketBase(const GameEnginePacketBase& _other) = delete; 
	GameEnginePacketBase(GameEnginePacketBase&& _other) = delete; 
	GameEnginePacketBase& operator=(const GameEnginePacketBase& _other) = delete;
	GameEnginePacketBase& operator=(const GameEnginePacketBase&& _other) = delete;

public:
	void Serialize();
	void Deserialize();

	unsigned char* GetSerializerDataPtr() { return serializer_.GetDataPtr(); }
	unsigned int GetSerializerSize() { return serializer_.GetOffSet(); }

	int GetPacketID() { return packetID_; }
	int GetPacketSize() { return packetSize_; }

	void SetPacketID(int _id) { packetID_ = _id; }
	void SetPacketSize(int _size) { packetSize_ = _size; }

protected:
	// 여기서 packetID_ 를 초기화 해 주세요.
	virtual void userSerialize() {};
	virtual void userDeserialize() {};

protected:
	int packetID_;
	int packetSize_;
	GameEngineSerializer serializer_;
};


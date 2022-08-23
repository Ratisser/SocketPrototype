#pragma once

#include <mutex>

#include "GameEnginePacketBase.h"

class GameEnginePacketHandler
{
public:
	GameEnginePacketHandler(); 
	~GameEnginePacketHandler();
	GameEnginePacketHandler(const GameEnginePacketHandler& _other) = delete; 
	GameEnginePacketHandler(GameEnginePacketHandler&& _other) = delete; 
	GameEnginePacketHandler& operator=(const GameEnginePacketHandler& _other) = delete;
	GameEnginePacketHandler& operator=(const GameEnginePacketHandler&& _other) = delete;

public:
	static GameEnginePacketHandler& GetInstance() { static GameEnginePacketHandler instance; return instance; }
	
public:
	GameEnginePacketBase* AnalyzePacket(char* _data, int _size);
	void PushPacket(GameEnginePacketBase* packet);
	
	void ProcessPacket();

private:
	std::vector<GameEnginePacketBase*> packetList_;
	std::mutex locker_;
};


#pragma once

class GameEnginePacket
{
public:
	GameEnginePacket(); 
	~GameEnginePacket();
	GameEnginePacket(const GameEnginePacket& _other) = delete; 
	GameEnginePacket(GameEnginePacket&& _other) = delete; 
	GameEnginePacket& operator=(const GameEnginePacket& _other) = delete;
	GameEnginePacket& operator=(const GameEnginePacket&& _other) = delete;

};


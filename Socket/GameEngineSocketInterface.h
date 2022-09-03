#pragma once

class GameEnginePacketBase;
class GameEngineSocketInterface
{
public:
	virtual void Send(GameEnginePacketBase* _packet) = 0;
	virtual void Send(SOCKET _receiver, GameEnginePacketBase* _packet) = 0;
};


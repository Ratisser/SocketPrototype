#pragma once

#include <mutex>
#include <unordered_map>
#include <queue>

#include "GameEnginePacketBase.h"
#include "ePacketID.h"

class GameEnginePacketHandler
{
public:
	GameEnginePacketHandler(bool _bServer);
	~GameEnginePacketHandler();
	GameEnginePacketHandler(const GameEnginePacketHandler& _other) = delete;
	GameEnginePacketHandler(GameEnginePacketHandler&& _other) = delete;
	GameEnginePacketHandler& operator=(const GameEnginePacketHandler& _other) = delete;
	GameEnginePacketHandler& operator=(const GameEnginePacketHandler&& _other) = delete;
	
public:
	// 패킷을 분석하고 푸쉬합니다.
	void AnalyzePacketAndPush(char* _data, int _size);

	// 패킷 오브젝트를 푸쉬합니다. 푸쉬된 오브젝트는 핸들러에서 생명주기를 관리합니다. (즉, 다른 곳에서 삭제하면 안 됩니다.)
	void PushPacket(GameEnginePacketBase* _packet);
	
	// 푸쉬된 패킷들을 순차처리합니다.
	void ProcessPacket(GameEngineSocketInterface* _network);

	void AddHandler(int _packetID, GameEnginePacketBase* _packetObject);

private:
	std::queue<GameEnginePacketBase*> packetQueue_;
	std::unordered_map<int, GameEnginePacketBase*> parents_;
	std::mutex locker_;

	bool bServer_;
};


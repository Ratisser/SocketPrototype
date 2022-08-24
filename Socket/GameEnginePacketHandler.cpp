#include "GameEnginePacketHandler.h"

#include <iostream>

#include "ChattingPacket.h"
#include "ePacketID.h"

GameEnginePacketHandler::GameEnginePacketHandler(bool _bServer)
	: bServer_(_bServer)
{
	// ���� �θ� ���̸� ��������
	parents_[ePacketID::Chat] = new ChattingPacket;
}

GameEnginePacketHandler::~GameEnginePacketHandler()
{
	while (!packetQueue_.empty())
	{
		GameEnginePacketBase* p = packetQueue_.front();
		delete p;
		packetQueue_.pop();
	}

	for (auto p : parents_)
	{
		delete p.second;
	}

	parents_.clear();
}

void GameEnginePacketHandler::AnalyzePacketAndPush(char* _data, int _size)
{
	GameEnginePacketBase* analyzedPacket = nullptr;

	ePacketID packetID = ePacketID::None;
	int packetSize = 0;

	// ��Ŷ ����� �����ɴϴ�.
	memcpy(&packetID, _data, sizeof(ePacketID));
	memcpy(&packetSize, _data + sizeof(ePacketID), sizeof(int));


	// �θ� �ʿ��� ã���� ��ü�� ������ �ִ� ���� �Դϴ�.
	auto findIter = parents_.find(packetID);
	if (findIter != parents_.end())
	{
		analyzedPacket = findIter->second->GetNewObject();
	}

	// �θ� �ʿ��� ã�ƿԴٸ� 
	if (nullptr != analyzedPacket)
	{
		analyzedPacket->GetSerializer().SetDataPtr(_data, _size);
		analyzedPacket->SetPacketID(packetID);
		PushPacket(analyzedPacket);
	}
}

void GameEnginePacketHandler::PushPacket(GameEnginePacketBase* packet)
{
	locker_.lock();
	packetQueue_.push(packet);
	locker_.unlock();
}

void GameEnginePacketHandler::ProcessPacket(GameEngineSocketInterface* _network)
{ 
	// ��Ŷ ó���߿� ��Ŷ�� �߰��ϸ� �� �ǹǷ� ��ٴ�.
	std::lock_guard<std::mutex> lg(locker_);

	while (!packetQueue_.empty())
	{
		GameEnginePacketBase* packet = packetQueue_.front();
		packetQueue_.pop();

		packet->Deserialize();
		packet->execute(bServer_, _network);

		delete packet;
	}
}
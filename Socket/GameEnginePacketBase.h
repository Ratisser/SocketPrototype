#pragma once

#include "ePacketID.h"
#include "GameEngineSerializer.h"
#include "GameEngineSocketInterface.h"

class GameEnginePacketHandler;
class GameEnginePacketBase
{
	friend GameEnginePacketHandler;
public:
	GameEnginePacketBase();
	virtual ~GameEnginePacketBase();
	GameEnginePacketBase(const GameEnginePacketBase& _other) = delete; 
	GameEnginePacketBase(GameEnginePacketBase&& _other) = delete; 
	GameEnginePacketBase& operator=(const GameEnginePacketBase& _other) = delete;
	GameEnginePacketBase& operator=(const GameEnginePacketBase&& _other) = delete;

public:
	void Serialize();
	void Deserialize();

public:
	ePacketID				GetPacketID()				{ return packetID_; }
	int						GetPacketSize()				{ return packetSize_; }

	GameEngineSerializer&	GetSerializer()				{ return serializer_; }

	void					SetPacketID(ePacketID _id)	{ packetID_ = _id; }
	void					SetPacketSize(int _size)	{ packetSize_ = _size; }

	GameEnginePacketBase*	GetNewObject()				{ return getUserObject(); }

protected:
	// ���⼭ packetID_ �� �ʱ�ȭ �� �ּ���.
	virtual void initPacketID() = 0;
	virtual void userSerialize() = 0;
	virtual void userDeserialize() = 0;
	// ���⼭ ���ο� ������Ʈ�� ��ȯ �� �ּ���.
	virtual GameEnginePacketBase* getUserObject() = 0;

	// ��Ŷ�� �� �ൿ�� �����ּ���. bool ���� �������� ����Ǵ����� ���� �����Դϴ�.
	virtual void execute(bool _bServer, GameEngineSocketInterface* _network) = 0;
protected:
	ePacketID packetID_;
	int packetSize_;
	GameEngineSerializer serializer_;
};


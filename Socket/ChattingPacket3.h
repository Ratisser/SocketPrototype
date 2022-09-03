#pragma once

#include "GameEnginePacketBase.h"

class ChattingPacket3 : public GameEnginePacketBase
{
public:
	ChattingPacket3(); 
	~ChattingPacket3();

	ChattingPacket3(const ChattingPacket3& _other) = delete; 
	ChattingPacket3(ChattingPacket3&& _other) = delete; 

	ChattingPacket3& operator=(const ChattingPacket3& _other) = delete;
	ChattingPacket3& operator=(const ChattingPacket3&& _other) = delete;

public:
	void SetText(const std::string& _text)
	{
		text_ = _text;
	}

protected:
	virtual void initPacketID() override;
	virtual void userSerialize() override;
	virtual void userDeserialize() override;
	virtual GameEnginePacketBase* getUserObject() override;
	virtual void execute(bool _bServer, GameEngineSocketInterface* _network) override;

private:
	std::string text_;
	int id_;
	float4 destination_;
};


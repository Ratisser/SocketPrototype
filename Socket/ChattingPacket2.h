#pragma once

#include "GameEnginePacketBase.h"

class ChattingPacket2 : public GameEnginePacketBase
{
public:
	ChattingPacket2(); 
	~ChattingPacket2();

	ChattingPacket2(const ChattingPacket2& _other) = delete; 
	ChattingPacket2(ChattingPacket2&& _other) = delete; 

	ChattingPacket2& operator=(const ChattingPacket2& _other) = delete;
	ChattingPacket2& operator=(const ChattingPacket2&& _other) = delete;

public:
	void SetText(const std::string& _text) { text_ = _text; }

protected:
	virtual void initPacketID() override;
	virtual void userSerialize() override;
	virtual void userDeserialize() override;
	virtual GameEnginePacketBase* getUserObject() override;
	virtual void execute(bool _bServer, GameEngineSocketInterface* _network) override;

private:
	std::string text_;
};


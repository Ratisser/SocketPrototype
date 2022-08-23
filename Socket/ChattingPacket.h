#pragma once

#include "GameEnginePacketBase.h"

class ChattingPacket : public GameEnginePacketBase
{
public:
	ChattingPacket(); 
	~ChattingPacket();
	ChattingPacket(const ChattingPacket& _other) = delete; 
	ChattingPacket(ChattingPacket&& _other) = delete; 
	ChattingPacket& operator=(const ChattingPacket& _other) = delete;
	ChattingPacket& operator=(const ChattingPacket&& _other) = delete;

public:
	void SetText(std::string& _text);
	std::string GetText() { return text_; }
protected:
	virtual void setPacketID();
	virtual void userSerialize() override;
	virtual void userDeserialize() override;

private:
	enum { MAX_TEXT_SIZE = 512 };
	std::string text_;
};

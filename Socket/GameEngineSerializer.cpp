#include "GameEngineSerializer.h"

void GameEngineSerializer::Read(void* Data, unsigned int _Size)
{
	memcpy_s(Data, _Size, &Data_[Offset_], _Size);
	Offset_ += _Size;
}

void GameEngineSerializer::Reset()
{
	Offset_ = 0;
	Data_.clear();
	Data_.resize(1024);
}

void GameEngineSerializer::OffsetReset()
{
	Offset_ = 0;
}

void GameEngineSerializer::Write(const void* Data, unsigned int _Size)
{
	if (Offset_ + _Size >= Data_.size())
	{
		Data_.resize(Data_.size() * 2);
	}

	memcpy_s(&Data_[Offset_], _Size, Data, _Size);
	Offset_ += _Size;
}

void GameEngineSerializer::operator<<(const int _Value)
{
	Write(&_Value, sizeof(int));
}

void GameEngineSerializer::operator<<(const float _Value)
{
	Write(&_Value, sizeof(float));
}

void GameEngineSerializer::operator<<(const ePacketID _value)
{
	Write(reinterpret_cast<const void*>(&_value), static_cast<unsigned int>(sizeof(ePacketID)));
}

void GameEngineSerializer::operator<<(const std::string& _Value)
{
	operator<<(static_cast<int>(_Value.size()));
	Write(reinterpret_cast<const void*>(&_Value[0]), static_cast<unsigned int>(_Value.size()));
}

void GameEngineSerializer::operator<<(const unsigned int _Value)
{
	Write(&_Value, sizeof(unsigned int));
}

void GameEngineSerializer::operator<<(const uint64_t& _Value)
{
	Write(&_Value, sizeof(uint64_t));
}

void GameEngineSerializer::operator<<(const float4& _Value)
{
	Write(&_Value, sizeof(float4));
}


void GameEngineSerializer::operator>>(ePacketID& _value)
{
	Read(reinterpret_cast<void*>(&_value), static_cast<unsigned int>(sizeof(ePacketID)));
}

void GameEngineSerializer::operator>>(std::string& _Value)
{
	int Size;
	operator>>(Size);
	_Value.resize(Size);
	Read(&_Value[0], Size);
}

void GameEngineSerializer::operator>>(int& _Value)
{
	Read(&_Value, sizeof(int));
}

void GameEngineSerializer::operator>>(unsigned int& _Value)
{
	Read(&_Value, sizeof(unsigned int));
}

void GameEngineSerializer::operator>>(float& _Value)
{
	Read(&_Value, sizeof(float));
}

void GameEngineSerializer::operator>>(float4& _Value)
{
	Read(&_Value, sizeof(float4));
}

void GameEngineSerializer::operator>>(uint64_t& _Value)
{
	Read(&_Value, sizeof(uint64_t));
}

GameEngineSerializer::GameEngineSerializer()
{
	Offset_ = 0;
	Data_.resize(1024);
}

GameEngineSerializer::GameEngineSerializer(const std::vector<unsigned char>& _Data)
	: Data_(_Data)
	, Offset_(0)
{

}
GameEngineSerializer::GameEngineSerializer(const char* _Data, unsigned int _Size)
	: Offset_(0)
{
	Data_.resize(_Size);

	memcpy_s(&Data_[0], _Size, _Data, _Size);
}

void GameEngineSerializer::SetDataPtr(const char* _data, unsigned int _size)
{
	Offset_ = 0;
	Data_.resize(_size);

	memcpy_s(&Data_[0], _size, _data, _size);
}
#pragma once
#include <vector>
#include <string>
#include "GameEngineMath.h"
#include "ePacketID.h"

class GameEngineSerializer
{
public:
	GameEngineSerializer();
	GameEngineSerializer(const std::vector<unsigned char>& _Data);
	GameEngineSerializer(const char* _Data, unsigned int _Size);

public:
	void Write(const void* Data, unsigned int _Size);
	void Read(void* Data, unsigned int _Size);

	void Reset();
	void OffsetReset();

	void SetDataPtr(const char* _data, unsigned int _size);

	unsigned int GetOffSet() { return Offset_; }
	const std::vector<unsigned char>& GetData() { return Data_; }
	unsigned char* GetDataPtr() { return &Data_[0]; }

public:
	void operator<<(const ePacketID _value);
	void operator<<(const std::string& _Value);
	void operator<<(const int _Value);
	void operator<<(const unsigned int _Value);
	void operator<<(const uint64_t& _Value);
	void operator<<(const float _Value);
	void operator<<(const float4& _Value);

	template<typename T>
	void WriteEnum(const T _Value);

	template<typename T>
	void WriteUserData(const T _Value);


	template<typename T>
	void WriteVector(std::vector<T>& _Value);

	void WriteVector(std::vector<std::string>& _Value)
	{
		operator<<(static_cast<int>(_Value.size()));
		for (size_t i = 0; i < _Value.size(); i++)
		{
			operator<<(_Value[i]);
		}
	}

	void WriteVector(std::vector<int>& _Value)
	{
		operator<<(static_cast<int>(_Value.size()));
		for (size_t i = 0; i < _Value.size(); i++)
		{
			operator<<(_Value[i]);
		}
	}


	void operator>>(ePacketID& _value);
	void operator>>(std::string& _Value);
	void operator>>(int& _Value);
	void operator>>(unsigned int& _Value);
	void operator>>(uint64_t& _Value);
	void operator>>(float& _Value);
	void operator>>(float4& _Value);

	template<typename T>
	void ReadEnum(T& _Value)
	{
		Read(reinterpret_cast<void*>(&_Value), static_cast<unsigned int>(sizeof(T)));
	}

	template<typename T>
	void ReadUserData(T& _Value)
	{
		Read(reinterpret_cast<void*>(&_Value), static_cast<unsigned int>(sizeof(T)));
	}

	template<typename T>
	void ReadVector(std::vector<T>& _Value)
	{
		int Size;
		operator>>(Size);
		_Value.resize(Size);
		for (size_t i = 0; i < _Value.size(); i++)
		{
			_Value[i].DeSerialize(*this);
		}
	}

	void ReadVector(std::vector<std::string>& _Value)
	{
		int Size;
		operator>>(Size);
		_Value.resize(Size);
		for (size_t i = 0; i < _Value.size(); i++)
		{
			operator>>(_Value[i]);
		}
	}

	void ReadVector(std::vector<int>& _Value)
	{
		int Size;
		operator>>(Size);
		_Value.resize(Size);
		for (size_t i = 0; i < _Value.size(); i++)
		{
			operator>>(_Value[i]);
		}
	}





private:
	unsigned int Offset_;
	std::vector<unsigned char> Data_;

};

template<typename T>
inline void GameEngineSerializer::WriteEnum(const T _Value)
{
	Write(reinterpret_cast<const void*>(&_Value), static_cast<unsigned int>(sizeof(T)));
}

template<typename T>
inline void GameEngineSerializer::WriteUserData(const T _Value)
{
	Write(reinterpret_cast<const void*>(&_Value), static_cast<unsigned int>(sizeof(T)));
}

template<typename T>
inline void GameEngineSerializer::WriteVector(std::vector<T>& _Value)
{
	operator<<(static_cast<int>(_Value.size()));
	for (size_t i = 0; i < _Value.size(); i++)
	{
		_Value[i].Serialize(*this);
		// Write(reinterpret_cast<const void*>(&_Value[i]), static_cast<unsigned int>(sizeof(T)));
	}
}

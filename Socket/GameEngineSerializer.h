#pragma once
#include <vector>
#include <string>
#include "GameEngineMath.h"

class GameEngineSerializer
{
public:
	GameEngineSerializer();
	GameEngineSerializer(const std::vector<unsigned char>& _Data);
	GameEngineSerializer(const char* _Data, unsigned int _Size);

public:
	unsigned int GetOffSet()
	{
		return Offset_;
	}

	void OffsetReset()
	{
		Offset_ = 0;
	}
	void Write(const void* Data, unsigned int _Size);
	void Read(void* Data, unsigned int _Size);

	void Reset()
	{
		Offset_ = 0;
		Data_.clear();
		Data_.resize(1024);
	}

	const std::vector<unsigned char>& GetData() {
		return Data_;
	}

	unsigned char* GetDataPtr() {
		return &Data_[0];
	}

public:
	// 이걸 그냥
	// 템플릿은 보통 좋은 선택이 아닙니다.
	void operator<<(const std::string& _Value);
	void operator<<(const int _Value);
	void operator<<(const unsigned int _Value);
	void operator<<(const uint64_t& _Value);
	void operator<<(const float _Value);
	void operator<<(const float4& _Value);

	template<typename T>
	void WriteEnum(const T _Value)
	{
		Write(reinterpret_cast<const void*>(&_Value), static_cast<unsigned int>(sizeof(T)));
	}

	template<typename T>
	void WriteUserData(const T _Value)
	{
		Write(reinterpret_cast<const void*>(&_Value), static_cast<unsigned int>(sizeof(T)));
	}


	template<typename T>
	void WriteVector(std::vector<T>& _Value)
	{
		operator<<(static_cast<int>(_Value.size()));
		for (size_t i = 0; i < _Value.size(); i++)
		{
			_Value[i].Serialize(*this);
			// Write(reinterpret_cast<const void*>(&_Value[i]), static_cast<unsigned int>(sizeof(T)));
		}
	}

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
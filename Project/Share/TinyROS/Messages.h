#pragma once
#include "TinyROSPlatformDef.h"
#include <string>
#include <typeinfo>
#include <typeindex>

namespace TinyROS
{
	struct SHA256Value
	{
		unsigned char value[32];
		bool operator==(SHA256Value& other);
		bool operator!=(SHA256Value& other);
		std::string ToHexString(bool uppercase = false);
	};

	class SHA256ValueComparator
	{
	public:
		bool operator()(const SHA256Value& left, const SHA256Value& right) const;
	};

	struct MasterBroadcastDatagram
	{
		int ListenPort;
		int Signal;	
	};

	using TypeID = std::type_index;
	using TypeIDHash = size_t;

	class Message
	{
	public:
		virtual TypeID GetTypeID() = 0;
		virtual std::string Serialize() = 0;
		virtual void Deserialize(std::string&) = 0;
		virtual ~Message() {}
	};


	// 直接使用对象的内存进行序列化/反序列化，
	// 因此，只能是一些简单的类型，不能包含指针、容器、静态成员等
	// 否则，行为是不可预料的
	template<typename TValue>
	class SimpleObjectMessage : public Message
	{
	public:
		virtual TypeID GetTypeID();
		virtual std::string Serialize();
		virtual void Deserialize(std::string& str);
		virtual ~SimpleObjectMessage() {}
	public:
		SimpleObjectMessage();
		SimpleObjectMessage(TValue value);
		TValue value;
	};

	// 模板类必须把定义和声明放在一起，不然gcc编译不过
	
	template<typename TValue>
	TypeID SimpleObjectMessage<TValue>::GetTypeID()
	{
		TypeID id(typeid(SimpleObjectMessage<TValue>));
		return id;
	}

	template<typename TValue>
	std::string SimpleObjectMessage<TValue>::Serialize()
	{
		constexpr int len = sizeof(TValue);
		char buf[len];
		char* pValue = reinterpret_cast<char*>(&this->value);
		std::copy(pValue, pValue + len, buf);
		return std::string(buf, len);
	}

	template<typename TValue>
	void SimpleObjectMessage<TValue>::Deserialize(std::string& str)
	{
		constexpr int len = sizeof(TValue);
		char* pValue = reinterpret_cast<char*>(&this->value);
		const char* buf = str.c_str();
		std::copy(buf, buf + len, pValue);
	}

	template<typename TValue>
	SimpleObjectMessage<TValue>::SimpleObjectMessage()
	{
		this->value = TValue();
	}

	template<typename TValue>
	SimpleObjectMessage<TValue>::SimpleObjectMessage(TValue value)
	{
		this->value = value;
	}

	// 使用Json进行编解码的消息
	// 内部拟用JsonCpp实现
	// 不能解决嵌套，除非调用也引入JsonCpp库，并且指定模板参数为Json::Value
	// 感觉没啥用，懒得实现了
	class JsonMessage : public Message
	{
	public:
		virtual TypeID GetTypeID();
		virtual std::string Serialize();
		virtual void Deserialize(std::string& str);
		virtual ~JsonMessage();
	public:
		template<typename T> T GetValueByName(std::string& name);
		template<typename T> T GetValueByName(const char* name);
		template<typename T> T SetValueByName(std::string& name, T value);
		template<typename T> T SetValueByName(const char*, T value);
		template<typename T> T& operator[] (std::string& name);
		template<typename T> T& operator[] (const char* name);
	private:
		class JsonMessageImplement;
		JsonMessageImplement* implData;
	};
}
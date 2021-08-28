#pragma once
#include "TinyROSPlatformDef.h"
#include "TinyROSDef.h"


namespace TinyROS
{

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
	inline TypeID SimpleObjectMessage<TValue>::GetTypeID()
	{
		TypeID id(typeid(SimpleObjectMessage<TValue>));
		return id;
	}

	template<typename TValue>
	inline std::string SimpleObjectMessage<TValue>::Serialize()
	{
		constexpr int len = sizeof(TValue);
		char buf[len];
		char* pValue = reinterpret_cast<char*>(&this->value);
		std::copy(pValue, pValue + len, buf);
		return std::string(buf, len);
	}

	template<typename TValue>
	inline void SimpleObjectMessage<TValue>::Deserialize(std::string& str)
	{
		constexpr int len = sizeof(TValue);
		char* pValue = reinterpret_cast<char*>(&this->value);
		const char* buf = str.c_str();
		std::copy(buf, buf + len, pValue);
	}

	template<typename TValue>
	inline SimpleObjectMessage<TValue>::SimpleObjectMessage()
	{
		this->value = TValue();
	}

	template<typename TValue>
	inline SimpleObjectMessage<TValue>::SimpleObjectMessage(TValue value)
	{
		this->value = value;
	}


	// 提供对简单类型的对象数组的支持
	// 数组不论多长，都属于同一种类型的消息，
	// 但长度为1的数组与简单对象消息属于不同类型的消息
	// 发布和订阅时，请注意消息类型的匹配
	template<typename TValue>
	class SimpleObjectArrayMessage : public Message
	{
	public:
		virtual TypeID GetTypeID();
		virtual std::string Serialize();
		virtual void Deserialize(std::string& str);
		virtual ~SimpleObjectArrayMessage();
	public:
		SimpleObjectArrayMessage();
		SimpleObjectArrayMessage(TValue* pValue, int count);
		SimpleObjectArrayMessage(const SimpleObjectArrayMessage& other);
		SimpleObjectArrayMessage(SimpleObjectArrayMessage&& other);
		SimpleObjectArrayMessage& operator= (const SimpleObjectArrayMessage& other);
		// 该方法直接访问内部数据，不建议使用
		// 不能删除通该方法返回的指针
		TValue* GetValuePointer();
		int GetCount();
		// 不进行越界检查，越界会造成未定义的结果
		TValue operator[](int index);
	private:
		TValue* pValue;
		int count;
	};

	template<typename TValue>
	inline TypeID SimpleObjectArrayMessage<TValue>::GetTypeID()
	{
		TypeID id(typeid(SimpleObjectArrayMessage<TValue>));
		return id;
	}

	template<typename TValue>
	inline std::string SimpleObjectArrayMessage<TValue>::Serialize()
	{
		if (this->count == 0)
		{
			return std::string();
		}
		constexpr int unitLen = sizeof(TValue);
		int totalLen = unitLen * this->count;
		char* p = reinterpret_cast<char*>(this->pValue);
		return std::string(p, totalLen);
	}

	template<typename TValue>
	inline void SimpleObjectArrayMessage<TValue>::Deserialize(std::string& str)
	{
		delete[] this->pValue;
		int count = str.size() / sizeof(TValue);
		if (count == 0)
		{
			this->pValue = nullptr;
			this->count = 0;
			return;
		}
		this->pValue = new TValue[count];
		this->count = count;
		char* p = reinterpret_cast<char*>(this->pValue);
		std::copy(str.c_str(), str.c_str() + sizeof(TValue) * count, p);
	}

	template<typename TValue>
	inline SimpleObjectArrayMessage<TValue>::~SimpleObjectArrayMessage()
	{
		delete[] this->pValue;
	}

	template<typename TValue>
	inline SimpleObjectArrayMessage<TValue>::SimpleObjectArrayMessage()
	{
		this->pValue = nullptr;
		this->count = 0;
	}

	template<typename TValue>
	inline SimpleObjectArrayMessage<TValue>::SimpleObjectArrayMessage(TValue* pValue, int count)
	{
		this->pValue = new TValue[count];
		this->count = count;
		std::copy(pValue, pValue + count, count);
	}

	template<typename TValue>
	inline SimpleObjectArrayMessage<TValue>::SimpleObjectArrayMessage(const SimpleObjectArrayMessage& other)
	{
		this->count = other.count;
		if (this->count == 0)
		{
			this->pValue = nullptr;
		}
		else
		{
			this->pValue = new TValue[this->count];
			std::copy(other.pValue, other.pValue + other.count, this->pValue);
		}
	}

	template<typename TValue>
	inline SimpleObjectArrayMessage<TValue>::SimpleObjectArrayMessage(SimpleObjectArrayMessage&& other)
	{
		this->count = other.count;
		this->pValue = other.pValue;
		other.pValue = nullptr;
	}

	template<typename TValue>
	inline SimpleObjectArrayMessage<TValue>& SimpleObjectArrayMessage<TValue>::operator=(const SimpleObjectArrayMessage& other)
	{
		delete[] this->pValue;
		// 长得和拷贝构造很像
		this->count = other.count;
		if (this->count == 0)
		{
			this->pValue = nullptr;
		}
		else
		{
			this->pValue = new TValue[count];
			std::copy(other.pValue, other.pValue + other.count, this->pValue);
		}
		return *this;
	}

	template<typename TValue>
	inline TValue* SimpleObjectArrayMessage<TValue>::GetValuePointer()
	{
		return this->pValue;
	}

	template<typename TValue>
	inline int SimpleObjectArrayMessage<TValue>::GetCount()
	{
		return this->count;
	}

	template<typename TValue>
	inline TValue SimpleObjectArrayMessage<TValue>::operator[](int index)
	{
		return this->pValue[index];
	}



	// 使用Json进行编解码的消息
	// 内部拟用JsonCpp实现
	// 不能解决嵌套，除非调用也引入JsonCpp库，并且指定模板参数为Json::Value
	// 感觉没啥用，懒得实现了
	// 也不能解决TypeID的问题，存在隐患
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
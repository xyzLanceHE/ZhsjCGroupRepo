#pragma once
#include "TinyROSPlatformDef.h"
#include "TinyROSDef.h"

namespace TinyROS
{
	class NoneType 
	{
		// 仅用于填充Subscriber的默认模板（填void会导致编译问题）
	};

	class ISubscriberInvoker 
	{
	private:
		// 用于回调Subscriber而不管模板参数
		friend class SubscriberImplement;
		virtual void Invoke(const char* buf, int len) = 0;
	};

	class SubscriberImplement
	{
	public:
		~SubscriberImplement();
		class SubscriberInnerNetwork;
	private:
		void Init();
		void OnRawMessageReceived(const char* buf, int len);
		ISubscriberInvoker* SubscriberInterface;
	private:
		// 这里写法与Publisher类似，详情见Publisher.h
		SubscriberImplement() = delete;
		SubscriberImplement(const char* topicName, TypeIDHash typeIdHash);
		template<typename TMessage, typename TCallbackObject> friend class Subscriber;
		SubscriberInnerNetwork* innerImpl;
	};

	template<typename TMessage, typename TCallbackObject = NoneType>
	class Subscriber : private ISubscriberInvoker
	{
	public:
		using MessageCallback = void (*)(TMessage);
		using MessageCallbackInObject = void (TCallbackObject::*)(TMessage);
	public:
		Subscriber(const char* topicName, MessageCallback callback);
		// 支持回调函数是对象的成员方法
		Subscriber(const char* topicName, TCallbackObject* executorObject, MessageCallbackInObject callback);
		Subscriber(const Subscriber&) = delete;
		Subscriber& operator=(const Subscriber&) = delete;
		~Subscriber();
	private:		
		MessageCallback UnrestrictedCallback;
		MessageCallbackInObject ObjectCallback;
		TCallbackObject* pCallbackObject;
		void OnMessageReceived(TMessage message);
		virtual void Invoke(const char* buf, int len);	// 通过 ISubscriberInvoker 继承
	private:
		SubscriberImplement* impl;
	};

	template<typename TMessage, typename TCallbackObject>
	inline Subscriber<TMessage, TCallbackObject>::Subscriber(const char* topicName, MessageCallback callback)
	{
		TypeID typedId(typeid(TMessage));
		TypeIDHash typeIdHash = typedId.hash_code();
		this->impl = new SubscriberImplement(topicName, typeIdHash);
		this->pCallbackObject = nullptr;
		this->ObjectCallback = nullptr;
		this->UnrestrictedCallback = callback;
	}

	template<typename TMessage, typename TCallbackObject>
	inline Subscriber<TMessage, TCallbackObject>::Subscriber(const char* topicName, TCallbackObject* executorObject, MessageCallbackInObject callback)
	{
		TypeID typedId(typeid(TMessage));
		TypeIDHash typeIdHash = typedId.hash_code();
		this->impl = new SubscriberImplement(topicName, typeIdHash);
		this->pCallbackObject = executorObject;
		this->ObjectCallback = callback;
		this->UnrestrictedCallback = nullptr;
	}

	template<typename TMessage, typename TCallbackObject>
	inline Subscriber<TMessage, TCallbackObject>::~Subscriber()
	{
		delete this->impl;
	}

	template<typename TMessage, typename TCallbackObject>
	inline void Subscriber<TMessage, TCallbackObject>::OnMessageReceived(TMessage msg)
	{
		if (this->pCallbackObject != nullptr)
		{
			(this->pCallbackObject->*(this->ObjectCallback))(msg);
		}
		else
		{
			(*this->UnrestrictedCallback)(msg);
		}
	}

	template<typename TMessage, typename TCallbackObject>
	inline void Subscriber<TMessage, TCallbackObject>::Invoke(const char* buf, int len) // 通过 ISubscriberInvoker 继承
	{
		std::string rawMsg(buf, len);
		delete[] buf;
		TMessage msg;
		msg.Deserialize(rawMsg);
		this->OnMessageReceived(msg);
	}

}
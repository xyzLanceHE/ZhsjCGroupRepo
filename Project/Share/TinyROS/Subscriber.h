#pragma once
#include "TinyROSPlatformDef.h"
#include "TinyROSDef.h"


namespace TinyROS
{



	class SubscriberImplement
	{
	public:
		~SubscriberImplement();
	private:
		using RawMessageCallback = void (*)(const char* buf, int len, void* msg);
		RawMessageCallback RawMessageReceived;
		void OnRawMessageReceived(const char* buf, int len);
	private:
		// 这里写法与Publisher类似，详情见Publisher.h
		SubscriberImplement();
		SubscriberImplement(const char* topicName);
		template<typename TMessage> friend class Subscriber;
		class SubscriberInnerNetwork;
		SubscriberInnerNetwork* innerImpl;
	};

	template<typename TMessage>
	class Subscriber
	{
	public:
		using MessageCallback = void (*)(TMessage);
		template<typename ObjectType>
		using MessageCallbackInObject = void (ObjectType::*)(TMessage);
	public:
		Subscriber(const char* topicName, MessageCallback callback);
		// 支持回调函数是对象的成员方法
		template<typename ObjectType>
		Subscriber(const char* topicName, ObjectType* executorObject, MessageCallbackInObject<ObjectType> callback);
		Subscriber(const Subscriber&) = delete;
		Subscriber& operator=(const Subscriber&) = delete;
		~Subscriber();
	private:
		SubscriberImplement* impl;
	public:
		void InvokeTest(int x)
		{
			char a[4];
			int* p = reinterpret_cast<int*>(a);
			*p = x;
			this->impl->OnRawMessageReceived(a, 4);
		}
	};



	template<typename TMessage>
	inline Subscriber<TMessage>::Subscriber(const char* topicName, MessageCallback callback)
	{
		this->impl = new SubscriberImplement(topicName);
		TMessage msg;
		auto rawCallback = ([](const char* buf, int len, void* msg) -> void
			{
				std::string rawMsg(buf, len);
				auto msg2 = *reinterpret_cast<TMessage*>(msg);
				msg2.Deserialize(rawMsg);
				
			});
		(*callback)(msg);
		this->impl->RawMessageReceived = *reinterpret_cast<SubscriberImplement::RawMessageCallback*>(&rawCallback);
	}


	template<typename TMessage>
	template<typename ObjectType>
	inline Subscriber<TMessage>::Subscriber(const char* topicName, ObjectType* executorObject, MessageCallbackInObject<ObjectType> callback)
	{

		this->impl = new SubscriberImplement(topicName);
		TMessage msg;
		auto rawCallback = [](const char* buf, int len, void* msg)
		{
			std::string rawMsg(buf, len);
			auto msg2 = *reinterpret_cast<TMessage*>(msg);
			msg2.Deserialize(rawMsg);		
		};
		(executorObject->*callback)(msg);
		this->impl->RawMessageReceived = *reinterpret_cast<SubscriberImplement::RawMessageCallback*>(&rawCallback);
	}


	template<typename TMessage>
	inline Subscriber<TMessage>::~Subscriber()
	{
		delete this->impl;
	}

}
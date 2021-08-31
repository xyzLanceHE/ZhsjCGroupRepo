#pragma once
#include "TinyROSPlatformDef.h"
#include "TinyROSDef.h"
#include "Exceptions.h"
#include "Messages.h"


namespace TinyROS
{
	class Subscriber
	{
	// 通过包装好的NewSubscriber函数获取对象指针
	private:
		Subscriber(const char* topicName, IMessageCallable* callbacks, Message* pTypedMessage);
		Subscriber() = delete;
		Subscriber(const Subscriber&) = delete;
		Subscriber(Subscriber&&) = delete;
		template<typename TMessage> friend Subscriber* NewSubscriber(const char* name, MessageCallback<TMessage>& callbacks);
	public:
		~Subscriber();
	public:
		class SubscriberImplement;
	private:
		SubscriberImplement* impl;
	};

	template<typename TMessage>
	Subscriber* NewSubscriber(const char* name, MessageCallback<TMessage>& callbacks)
	{
		TMessage* pTemp = new TMessage();
		Message* pMsg = dynamic_cast<Message*>(pTemp);
		IMessageCallable* pCallbacks = &callbacks;
		return new Subscriber(name, pCallbacks, pMsg);
	}

}


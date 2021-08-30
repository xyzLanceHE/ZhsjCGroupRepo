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
		Subscriber(const char* topicName, TypeIDHash msgType, MessageCallback& callbacks);
		Subscriber() = delete;
		Subscriber(const Subscriber&) = delete;
		Subscriber(Subscriber&&) = delete;
		template<typename TMessage> friend Subscriber* NewSubscriber(const char* name, MessageCallback& callbacks);
	public:
		~Subscriber();
	public:
		class SubscriberImplement;
	private:
		SubscriberImplement* impl;
	};

	template<typename TMessage>
	Subscriber* NewSubscriber(const char* name, MessageCallback& callbacks)
	{
		TMessage* pTemp = new TMessage();
		Message* pMsg = dynamic_cast<Message*>(pTemp);
		TypeIDHash msgType = pMsg->GetTypeID();
		return new Subscriber(name, msgType, callbacks);
	}

}


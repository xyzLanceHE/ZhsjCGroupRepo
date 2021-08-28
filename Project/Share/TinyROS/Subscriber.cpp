#include "Subscriber.h"
#include "Node.h"
#include "Exceptions.h"
#include "TinyROSNetPrefix.h"

#include <iostream>
#include <thread>
#include <mutex>

namespace TinyROS
{
	class SubscriberImplement::SubscriberInnerNetwork
	{
	public:
		SOCKET SubscribeSocketFD;
		std::string TopicName;
	public:
		SubscriberInnerNetwork(const char* topicName, TypeIDHash typeIdHash);
	};


	SubscriberImplement::~SubscriberImplement()
	{
		delete this->innerImpl;
	}

	SubscriberImplement::SubscriberImplement(const char* topicName, TypeIDHash typeIdHash)
	{
		this->innerImpl = new SubscriberInnerNetwork(topicName, typeIdHash);
	}

	void SubscriberImplement::SubscribeThread()
	{

	}

	// buf是SubscribeThread的资源，由Invoke在完成复制之后删除
	void SubscriberImplement::OnRawMessageReceived(const char* buf, int len)
	{
		this->SubscriberInterface->Invoke(buf, len);
	}

}


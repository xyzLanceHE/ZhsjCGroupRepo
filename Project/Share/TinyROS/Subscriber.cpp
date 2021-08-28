#include "Subscriber.h"


namespace TinyROS
{
	SubscriberImplement::~SubscriberImplement()
	{

	}

	SubscriberImplement::SubscriberImplement()
	{

	}

	SubscriberImplement::SubscriberImplement(const char* topicName)
	{

	}

	void SubscriberImplement::OnRawMessageReceived(const char* buf, int len)
	{
		(*(this->RawMessageReceived))(buf, len, nullptr);
	}

}


#include "Publisher.h"
#include "Node.h"
#include "Exceptions.h"
#include "TinyROSNetPrefix.h"

#include <iostream>
#include <thread>
#include <mutex>

namespace TinyROS
{
	class PublisherImplement::PublisherInnerNetwork
	{
	public:
		SOCKET PublishSocketFD;
		std::string TopicName;
	public:
		PublisherInnerNetwork(const char* topicName, TypeIDHash typdIdHash);
	};

	PublisherImplement::~PublisherImplement()
	{
		delete this->innerImpl;
	}


	PublisherImplement::PublisherImplement(const char* topicName, TypeIDHash typdIdHash)
	{
		this->innerImpl = new PublisherInnerNetwork(topicName, typdIdHash);
	}

	// buf是调用方的string的资源，由于socket另起线程，需要复制一份，防止资源被调用方回收
	void PublisherImplement::InnerPublish(const char* buf, int len)
	{

	}

	PublisherImplement::PublisherInnerNetwork::PublisherInnerNetwork(const char* topicName, TypeIDHash typdIdHash)
	{
		this->TopicName = std::string(topicName);
	}

}
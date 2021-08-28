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
		TypeIDHash TypeIDHashVal;
		std::string BroadcastIP;
		TopicPort BroadcastPort;
		std::string LocalIP;
		std::mutex PublishMutex;
		~PublisherInnerNetwork();
	private:
		friend class PublisherImplement;
		PublisherInnerNetwork(const char* topicName, TypeIDHash typdIdHash);
		void InnerInit();
	};

	PublisherImplement::~PublisherImplement()
	{
		delete this->innerImpl;
	}


	PublisherImplement::PublisherImplement(const char* topicName, TypeIDHash typdIdHash)
	{
		this->innerImpl = new PublisherInnerNetwork(topicName, typdIdHash);
	}

	void PublisherImplement::Init()
	{
		this->innerImpl->InnerInit();
	}

	// buf是调用方的string的资源，由于socket另起线程，需要复制一份，防止资源被调用方回收
	void PublisherImplement::InnerPublish(const char* buf, int len)
	{

	}

	PublisherImplement::PublisherInnerNetwork::PublisherInnerNetwork(const char* topicName, TypeIDHash typdIdHash)
	{
		this->TopicName = std::string(topicName);
		this->TypeIDHashVal = typdIdHash;
	}

	PublisherImplement::PublisherInnerNetwork::~PublisherInnerNetwork()
	{
		CloseSocket(this->PublishSocketFD);
	}

	void PublisherImplement::PublisherInnerNetwork::InnerInit()
	{
		this->BroadcastIP = NodeInnerMethods::GetBroadcastIP();
		this->BroadcastPort = NodeInnerMethods::RequestTopic(this->TopicName.c_str(), RequestPublish, this->TypeIDHashVal);
		this->LocalIP = NodeInnerMethods::GetLocalIP();
		
	}

}
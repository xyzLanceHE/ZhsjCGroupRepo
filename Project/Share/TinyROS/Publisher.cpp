#include "Publisher.h"
#include "Node.h"
#include "Exceptions.h"
#include "TinyROSNetPrefix.h"

#include <iostream>
#include <thread>
#include <mutex>

namespace TinyROS
{
	//class PublisherImplement::PublisherInnerNetwork
	//{
	//public:
	//	SOCKET PublishSocketFD;
	//	std::string TopicName;
	//	TypeIDHash TypeIDHashVal;
	//	std::string BroadcastIP;
	//	TopicPort BroadcastPort;
	//	std::string LocalIP;
	//	std::mutex PublishMutex;
	//	~PublisherInnerNetwork();
	//private:
	//	friend class PublisherImplement;
	//	PublisherInnerNetwork(const char* topicName, TypeIDHash typdIdHash);
	//	void InnerInit();
	//	void PublishThread(char* buf, int len);
	//	sockaddr_in addrCache;
	//};

	//PublisherImplement::~PublisherImplement()
	//{
	//	delete this->innerImpl;
	//}


	//PublisherImplement::PublisherImplement(const char* topicName, TypeIDHash typdIdHash)
	//{
	//	this->innerImpl = new PublisherInnerNetwork(topicName, typdIdHash);
	//}

	//void PublisherImplement::Init()
	//{
	//	this->innerImpl->InnerInit();
	//}

	//// buf是调用方的string的资源，由于socket另起线程，需要复制一份，防止资源被调用方回收
	//void PublisherImplement::InnerPublish(const char* buf, int len)
	//{
	//	char* innerBuf = new char[len]; // 该buf由PublishThread回收
	//	std::copy(buf, buf + len, innerBuf);
	//	std::thread pubThread(&PublisherInnerNetwork::PublishThread, this->innerImpl, innerBuf, len);
	//	pubThread.detach();
	//}

	//PublisherImplement::PublisherInnerNetwork::PublisherInnerNetwork(const char* topicName, TypeIDHash typdIdHash)
	//{
	//	this->TopicName = std::string(topicName);
	//	this->TypeIDHashVal = typdIdHash;
	//}

	//PublisherImplement::PublisherInnerNetwork::~PublisherInnerNetwork()
	//{
	//	CloseSocket(this->PublishSocketFD);
	//}

	//void PublisherImplement::PublisherInnerNetwork::InnerInit()
	//{
	//	this->BroadcastIP = NodeInnerMethods::GetBroadcastIP();
	//	this->BroadcastPort = NodeInnerMethods::RequestTopic(this->TopicName.c_str(), RequestPublish, this->TypeIDHashVal);
	//	this->LocalIP = NodeInnerMethods::GetLocalIP();
	//	
	//	this->PublishSocketFD = socket(AF_INET, SOCK_DGRAM, 0);
	//	if (this->PublishSocketFD == INVALID_SOCKET)
	//	{
	//		throw CommunicateException("此Publisher未能初始化Socket");
	//	}

	//	int ret;

	//	int reuseAddr = 1;
	//	ret = setsockopt(this->PublishSocketFD, SOL_SOCKET, SO_REUSEADDR,
	//		reinterpret_cast<char*>(&reuseAddr), sizeof(reuseAddr));
	//	if (ret != 0)
	//	{
	//		throw CommunicateException("此Publisher未能复用端口");
	//	}

	//	sockaddr_in localBindAddr;
	//	localBindAddr.sin_family = AF_INET;
	//	localBindAddr.sin_port = htons(this->BroadcastPort);
	//	inet_pton(AF_INET, this->LocalIP.c_str(), reinterpret_cast<char*>(&localBindAddr.sin_addr.s_addr));
	//	ret = bind(this->PublishSocketFD, reinterpret_cast<sockaddr*>(&localBindAddr),
	//		sizeof(localBindAddr));
	//	if (ret != 0)
	//	{
	//		throw CommunicateException("此Publisher未能绑定端口");
	//	}

	//	this->addrCache.sin_family = AF_INET;
	//	this->addrCache.sin_port = htons(this->BroadcastPort);
	//	inet_pton(AF_INET, this->BroadcastIP.c_str(), reinterpret_cast<char*>(&addrCache.sin_addr.s_addr));
	//}

	//void PublisherImplement::PublisherInnerNetwork::PublishThread(char* buf, int len)
	//{
	//	this->PublishMutex.lock();
	//	
	//	int sentLen = sendto(this->PublishSocketFD, buf, len, 0,
	//		reinterpret_cast<sockaddr*>(&this->addrCache), sizeof(sockaddr_in));
	//	if (sentLen == -1)
	//	{

	//	}
	//	else
	//	{

	//	}

	//	this->PublishMutex.unlock();
	//	delete[] buf;
	//}

}
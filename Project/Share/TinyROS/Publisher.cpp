#include "Publisher.h"
#include "Node.h"
#include "Exceptions.h"
#include "TinyROSNetPrefix.h"

#include <iostream>
#include <thread>
#include <mutex>

namespace TinyROS
{
	class Publisher::PublisherImplement
	{
	public:
		SOCKET PublishSocketFD;
		std::string TopicName;
		TypeIDHash TypeIDHashVal;
		std::string BroadcastIP;
		TopicPort BroadcastPort;
		std::string LocalIP;
		std::mutex PublishMutex;
		sockaddr_in addrCache;
	public:
		~PublisherImplement();
		void PublishThread(char* buf, int len);
		void Init();
	};

	Publisher::Publisher(const char* topicName, TypeIDHash msgType)
	{
		this->impl = new PublisherImplement();
		this->impl->TopicName = topicName;
		this->impl->TypeIDHashVal = msgType;
		try
		{
			this->impl->Init();
		}
		catch (TinyROSException& e)
		{
			delete this->impl;
			throw e;
		}
	}

	Publisher::~Publisher()
	{
		delete this->impl;
	}

	void Publisher::Publish(Message& msg)
	{
		std::string s = msg.Serialize();
		int len = s.size();
		char* buf = new char[len]; // 该buf由PublishThread回收
		std::copy(s.begin(), s.end(), buf);
		std::thread pubThread(&PublisherImplement::PublishThread, this->impl, buf, len);
		pubThread.detach();
	}

	void Publisher::PublisherImplement::PublishThread(char* buf, int len)
	{
		this->PublishMutex.lock();

		int sentLen = sendto(this->PublishSocketFD, buf, len, 0,
			reinterpret_cast<sockaddr*>(&this->addrCache), sizeof(sockaddr_in));
		if (sentLen == -1)
		{

		}
		else
		{

		}
		this->PublishMutex.unlock();
		delete[] buf;
	}

	void Publisher::PublisherImplement::Init()
	{
		this->BroadcastIP = NodeInnerMethods::GetBroadcastIP();
		this->BroadcastPort = NodeInnerMethods::RequestTopic(this->TopicName.c_str(), RequestPublish, this->TypeIDHashVal);
		this->LocalIP = NodeInnerMethods::GetLocalIP();

		this->PublishSocketFD = socket(AF_INET, SOCK_DGRAM, 0);
		if (this->PublishSocketFD == INVALID_SOCKET)
		{
			throw CommunicateException("此Publisher未能初始化Socket");
		}

		int ret;

		int reuseAddr = 1;
		ret = setsockopt(this->PublishSocketFD, SOL_SOCKET, SO_REUSEADDR,
			reinterpret_cast<char*>(&reuseAddr), sizeof(reuseAddr));
		if (ret != 0)
		{
			throw CommunicateException("此Publisher未能复用端口");
		}

		sockaddr_in localBindAddr;
		localBindAddr.sin_family = AF_INET;
		localBindAddr.sin_port = htons(this->BroadcastPort);
		inet_pton(AF_INET, this->LocalIP.c_str(), reinterpret_cast<char*>(&localBindAddr.sin_addr.s_addr));
		ret = bind(this->PublishSocketFD, reinterpret_cast<sockaddr*>(&localBindAddr),
			sizeof(localBindAddr));
		if (ret != 0)
		{
			throw CommunicateException("此Publisher未能绑定端口");
		}

		this->addrCache.sin_family = AF_INET;
		this->addrCache.sin_port = htons(this->BroadcastPort);
		inet_pton(AF_INET, this->BroadcastIP.c_str(), reinterpret_cast<char*>(&addrCache.sin_addr.s_addr));
	}

	Publisher::PublisherImplement::~PublisherImplement()
	{
		CloseSocket(this->PublishSocketFD);
	}
}
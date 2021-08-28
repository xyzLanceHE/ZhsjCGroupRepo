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
		TypeIDHash TypeIDHashVal;
		std::string BroadcastIP;
		TopicPort BroadcastPort;
		std::string LocalIP;
		~SubscriberInnerNetwork();
	private:
		friend class SubscriberImplement;
		SubscriberImplement* pOwner;
		SubscriberInnerNetwork(const char* topicName, TypeIDHash typeIdHash);
		void InnerInit();
		void SubscribeThread();
	};


	SubscriberImplement::~SubscriberImplement()
	{
		delete this->innerImpl;
	}

	SubscriberImplement::SubscriberImplement(const char* topicName, TypeIDHash typeIdHash)
	{
		this->innerImpl = new SubscriberInnerNetwork(topicName, typeIdHash);
		this->innerImpl->pOwner = this;
	}

	void SubscriberImplement::Init()
	{
		this->innerImpl->InnerInit();
	}

	// buf是SubscribeThread的资源，由Invoke在完成复制之后删除
	void SubscriberImplement::OnRawMessageReceived(const char* buf, int len)
	{
		this->SubscriberInterface->Invoke(buf, len);
	}

	SubscriberImplement::SubscriberInnerNetwork::SubscriberInnerNetwork(const char* topicName, TypeIDHash typeIdHash)
	{
		this->TopicName = std::string(topicName);
		this->TypeIDHashVal = typeIdHash;
	}

	SubscriberImplement::SubscriberInnerNetwork::~SubscriberInnerNetwork()
	{
		CloseSocket(this->SubscribeSocketFD);
	}

	void SubscriberImplement::SubscriberInnerNetwork::InnerInit()
	{
		this->BroadcastPort = NodeInnerMethods::RequestTopic(this->TopicName.c_str(), RequestPublish, this->TypeIDHashVal);
		this->SubscribeSocketFD = socket(AF_INET, SOCK_DGRAM, 0);
		if (this->SubscribeSocketFD == INVALID_SOCKET)
		{
			throw CommunicateException("此Subscriber未能初始化Socket");
		}

		int ret;

		int reuseAddr = 1;
		ret = setsockopt(this->SubscribeSocketFD, SOL_SOCKET, SO_REUSEADDR,
			reinterpret_cast<char*>(&reuseAddr), sizeof(reuseAddr));
		if (ret != 0)
		{
			throw CommunicateException("此Subscriber未能复用端口");
		}

		sockaddr_in localBindAddr;
		localBindAddr.sin_family = AF_INET;
		localBindAddr.sin_port = htons(this->BroadcastPort);
		localBindAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		ret = bind(this->SubscribeSocketFD, reinterpret_cast<sockaddr*>(&localBindAddr),
			sizeof(localBindAddr));
		if (ret != 0)
		{
			throw CommunicateException("此Subscriber未能绑定端口");
		}
		
		this->LocalIP = NodeInnerMethods::GetLocalIP();
		this->BroadcastIP = NodeInnerMethods::GetBroadcastIP();
		ip_mreq multicastOption;
		inet_pton(AF_INET, this->BroadcastIP.c_str(), &multicastOption.imr_multiaddr);
		inet_pton(AF_INET, this->LocalIP.c_str(), &multicastOption.imr_interface);
		ret = setsockopt(this->SubscribeSocketFD, IPPROTO_IP, IP_ADD_MEMBERSHIP,
			reinterpret_cast<char*>(&multicastOption), sizeof(multicastOption));
		if (ret != 0)
		{
			throw CommunicateException("此Subscriber未能加入组播");
		}

		std::thread subscribeThread(&SubscriberInnerNetwork::SubscribeThread, this);
		subscribeThread.detach();
	}

	void SubscriberImplement::SubscriberInnerNetwork::SubscribeThread()
	{
		std::cout << "此Subscriber开始收听" << this->TopicName << std::endl;
		while (true)
		{
			char* buf = new char[1024];
			int len = recvfrom(this->SubscribeSocketFD, buf, 1024, 0,
				nullptr, nullptr);
			if (len == -1)
			{

			}
			else
			{
				std::thread handleThread(&SubscriberImplement::OnRawMessageReceived, this->pOwner, buf, len);
				handleThread.detach();	
			}
		}
	}
}


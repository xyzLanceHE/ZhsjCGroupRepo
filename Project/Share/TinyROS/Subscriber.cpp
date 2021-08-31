#include "Subscriber.h"
#include "Node.h"
#include "Exceptions.h"
#include "TinyROSNetPrefix.h"

#include <iostream>
#include <thread>
#include <mutex>

namespace TinyROS
{
	class Subscriber::SubscriberImplement
	{
	public:
		SOCKET SubscribeSocketFD;
		std::string TopicName;
		TypeIDHash TypeIDHashVal;
		std::string BroadcastIP;
		TopicPort BroadcastPort;
		std::string LocalIP;
		IMessageCallable* pCallback;
		Message* pTypedMessage;
	public:
		~SubscriberImplement();
		void Init();
		void SubscribeThread();
		void InvokeCallback(const char* buf, int len);
	};

	Subscriber::Subscriber(const char* topicName, IMessageCallable* callbacks, Message* pTypedMessage)
	{
		this->impl = new SubscriberImplement();
		this->impl->TopicName = topicName;
		TypeIDHash hash = pTypedMessage->GetTypeID();
		this->impl->TypeIDHashVal = hash;
		this->impl->pCallback = callbacks;
		this->impl->pTypedMessage = pTypedMessage;
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

	Subscriber::~Subscriber()
	{
		delete this->impl;
	}

	void Subscriber::SubscriberImplement::SubscribeThread()
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
				// std::cout << "received " << len << " bytes\n";
				std::thread handleThread(&SubscriberImplement::InvokeCallback, this, buf, len);
				handleThread.detach();
			}
		}
	}

	void Subscriber::SubscriberImplement::InvokeCallback(const char* buf, int len)
	{
		std::string s(buf, len);
		Message* pMsg = this->pTypedMessage->NewDeserialize(s);
		Message& msg = *pMsg;
		this->pCallback->Invoke(msg);
		delete pMsg;
		delete[] buf;
	}

	void Subscriber::SubscriberImplement::Init()
	{
		this->BroadcastPort = NodeInnerMethods::RequestTopic(this->TopicName.c_str(), RequestSubscribe, this->TypeIDHashVal);
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

		std::thread subscribeThread(&SubscriberImplement::SubscribeThread, this);
		subscribeThread.detach();
	}

	Subscriber::SubscriberImplement::~SubscriberImplement()
	{
		CloseSocket(this->SubscribeSocketFD);
	}

}


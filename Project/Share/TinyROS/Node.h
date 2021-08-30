#pragma once
#include "TinyROSPlatformDef.h"
#include "Subscriber.h"
#include "Publisher.h"
#include "TinyROSDef.h"

namespace TinyROS
{

	class Node
	{
	public:
		static void Init(const char* name);
		static void Init(const char* name, const char* configPath);
		static void Close();
	public:
		static bool IsInitialized;
	private:
		static void SetUpSocket();
		static void SetUpNameHash();
		static void ScanForMaster();
		static void LoadIPList();
		static void RegisterSelf();
		static void UnregisterSelf();
		static void MasterReceiveThread();
	private:
		class NodeImplementData;
		static NodeImplementData* const implData;
		friend class NodeInnerMethods;
	};

	class NodeInnerMethods
	{
	private:
		static TopicPort RequestTopic(const char* topicName, int requestType, TypeIDHash topicType, bool createIfNotExist = true);
		static const char* GetBroadcastIP();
		static const char* GetLocalIP();
		friend class Publisher::PublisherImplement;
		friend class Subscriber::SubscriberImplement;
	};
}
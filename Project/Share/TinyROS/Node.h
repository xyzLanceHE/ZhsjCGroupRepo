#pragma once
#include "TinyROSPlatformDef.h"
#include "Subscriber.h"
#include "Publisher.h"

namespace TinyROS
{
	using TopicID = int;

	class Node
	{
	public:
		static void Init(const char* name);
		static void Init(const char* name, const char* configPath);
		static void HashTest();
	public:
		static bool IsInitialized;
	private:
		static void SetUpSocket();
		static void SetUpNameHash();
		static void ScanForMaster();
		static void LoadIPList();
		static void MasterReceiveThread();
	private:
		class NodeImplementData;
		static NodeImplementData* const implData;
		friend class NodeInnerMethods;
	};

	class NodeInnerMethods
	{
	private:
		static TopicID RequestTopic(const char* topicName);
		static TopicID GetTopic(const char* topicName);
		template<typename TMessage> friend class Subscriber;
		template<typename TMessage> friend class Publisher;
	};
}
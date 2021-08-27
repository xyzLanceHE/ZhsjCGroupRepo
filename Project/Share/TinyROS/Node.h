#pragma once
#include "TinyROSPlatformDef.h"
#include "Subscriber.h"
#include "Publisher.h"

namespace TinyROS
{
	using TopicID = int;

	constexpr int RequestRegister = 11000 + 0b000;
	constexpr int RequestPublish = 11000 + 0b011;
	constexpr int RequestSubscribe = 11000 + 0b110;
	constexpr int RequestUnregister = 11000 + 0b101;

	constexpr int RequestSuccess = 0;
	constexpr int RequestFail = -1;

	constexpr int RequestTooLong = -10001;
	constexpr int RequestUnknown = -10002;

	constexpr int RegisterNameDuplicate = -10101;
	constexpr int RegisterBadCheck = -10102;

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
		static TopicID RequestTopic(const char* topicName);
		static TopicID GetTopic(const char* topicName);
		template<typename TMessage> friend class Subscriber;
		template<typename TMessage> friend class Publisher;
	};
}
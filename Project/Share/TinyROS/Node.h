#pragma once
#include "TinyROSPlatformDef.h"
#include "Subscriber.h"
#include "Publisher.h"
#include "Messages.h"

namespace TinyROS
{
	using TopicPort = int;

	constexpr int RequestRegister = 11000 + 0b000;
	constexpr int RequestPublish = 11000 + 0b011;
	constexpr int RequestSubscribe = 11000 + 0b110;
	constexpr int RequestUnregister = 11000 + 0b101;

	constexpr int RequestSuccess = 0;
	constexpr int RequestFail = -1;

	constexpr int RequestTooLong = -10001;
	constexpr int RequestUnknown = -10002;

	constexpr int RegisterBadCheck = -10101;
	constexpr int RegisterNameDuplicate = -10102;

	constexpr int TopicNameBadCheck = -10201;
	constexpr int TopicNotExist = -10202;
	constexpr int TopicTypeError = -10203;
	constexpr int AlreadyPubToTopic = -10204;
	constexpr int AlreadySubFromTopic = -10205;

	constexpr int UnregisteredNode = -10301;

	constexpr int HeadLen = sizeof(int);
	constexpr int HashLen = sizeof(SHA256Value);
	constexpr int TopicTypeLen = sizeof(TypeIDHash);
	constexpr int FlagLen = sizeof(bool);

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
		static TopicPort RequestTopic(const char* topicName, int type, TypeIDHash topicType, bool createIfNotExist = true);
		template<typename TMessage> friend class Subscriber;
		template<typename TMessage> friend class Publisher;
	};
}
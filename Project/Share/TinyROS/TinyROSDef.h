#pragma once
#include <string>
#include <typeinfo>
#include <typeindex>

namespace TinyROS
{
	struct SHA256Value
	{
		unsigned char value[32];
		bool operator==(SHA256Value& other);
		bool operator!=(SHA256Value& other);
		std::string ToHexString(bool uppercase = false);
	};

	using TypeID = std::type_index;
	using TypeIDHash = size_t;
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
}
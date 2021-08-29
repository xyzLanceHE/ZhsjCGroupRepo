#pragma once
#include <string>

namespace TinyROS
{
	struct SHA256Value
	{
		unsigned char value[32];
		SHA256Value() {};
		SHA256Value(const SHA256Value& other);
		SHA256Value& operator=(SHA256Value& other);
		bool operator==(SHA256Value& other);
		bool operator!=(SHA256Value& other);
		std::string ToHexString(bool uppercase = false);
	};

	using TypeIDHash = SHA256Value;
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
	constexpr int TopicTypeLen = sizeof(SHA256Value);
	constexpr int FlagLen = sizeof(bool);
}
#pragma once
#include "TinyROSPlatformDef.h"
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

	struct MasterBroadcastDatagram
	{
		int ListenPort;
		int Signal;	
	};

	using TypeID = std::type_index;
	using TypeIDHash = size_t;

	class Message
	{
	public:
		virtual TypeID GetTypeID();
		virtual ~Message() {}
	};

}
#pragma once
#include "TinyROSPlatformDef.h"
#include <string>

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
}
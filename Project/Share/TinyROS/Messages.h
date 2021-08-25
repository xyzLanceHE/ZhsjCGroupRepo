#pragma once
#include "TinyROSPlatformDef.h"

namespace TinyROS
{
	struct SHA256Value
	{
		unsigned char value[32];
		bool operator==(SHA256Value& other);
		bool operator!=(SHA256Value& other);
	};

	struct MasterBroadcastDatagram
	{
		int ListenPort;
		int Signal;	
	};
}
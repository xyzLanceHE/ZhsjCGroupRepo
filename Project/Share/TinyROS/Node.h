#pragma once
#include "TinyROSPlatformDef.h"

namespace TinyROS
{


	class Node
	{
	public:
		static void Init(const char* name);
		static void HashTest();
	public:
		static bool IsInitialized;
	private:
		static void ScanForMaster();
		static void LoadIPList();
	private:
		class NodeImplementData;
		static NodeImplementData* const implData;
	};
}
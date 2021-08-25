#pragma once
#include "TinyROSPlatformDef.h"

namespace TinyROS
{
	class Master
	{
	public:
		static void Run();
		static void Exit();
		static void LoadConfig(const char* configPath);
		static void SaveConfig(const char* configPath);
	private:
		static bool ExistOtherMaster();
		static void BroadcastThread();
	private:
		class MasterImplementData;
		static MasterImplementData* const implData;
	};



}
#pragma once
#include "TinyROSPlatformDef.h"

namespace TinyROS
{
	class Master
	{
	public:
		static void Run();
		static void LoadConfig(const char* configPath);
		static void SaveConfig(const char* configPath);
	private:
		class MasterImplementData;
		static MasterImplementData* implData;
	};
}
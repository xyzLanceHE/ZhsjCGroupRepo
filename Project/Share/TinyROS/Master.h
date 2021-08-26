#pragma once
#include "TinyROSPlatformDef.h"

namespace TinyROS
{
	class Master
	{
	public:
		static void Run();
		static void Spin();
		static void Exit();
		static void LoadConfig(const char* configPath);
		static void SaveConfig(const char* configPath);
	private:
		static bool ExistOtherMaster();
		static void SetUpSocket();
		static void BroadcastThread();
		static void ListenThread();
		static void TakeOverCtrlC();
		static void CtrlCHandler(int signal);
	private:
		class MasterImplementData;
		static MasterImplementData* const implData;
	};



}
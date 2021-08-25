#include "Master.h"
#include "Messages.h"
#include "Exceptions.h"

#ifndef __TINYROS_PLATFORM__
#error 无效的平台信息
#endif

#if __TINYROS_ON_LINUX_PRIDEF__
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <netinet/in.h>
	#include <ifaddrs.h>
	using SOCKET = int;
#elif __TINYROS_ON_WINDOWS_PRIDEF__
	#include <WinSock2.h>
	#include <Iphlpapi.h>
	#pragma comment(lib, "WS2_32.lib")
	#pragma comment(lib, "Iphlpapi.lib")
#endif

namespace TinyROS
{
	class Master::MasterImplementData
	{

	};

	Master::MasterImplementData* Master::implData = new Master::MasterImplementData();

	void Master::Run()
	{

	}

	void Master::LoadConfig(const char* configPath)
	{

	}
	void Master::SaveConfig(const char* configPath)
	{

	}
}
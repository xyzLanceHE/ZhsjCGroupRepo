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
	#define INVALID_SOCKET -1
#elif __TINYROS_ON_WINDOWS_PRIDEF__
	#include <WinSock2.h>
	#include <Iphlpapi.h>
	#pragma comment(lib, "WS2_32.lib")
	#pragma comment(lib, "Iphlpapi.lib")
#endif

#include <iostream>
#include <fstream>
#include "JsonCpp/include/json.h"
#include <vector>
#include <algorithm>

namespace TinyROS
{
	class Master::MasterImplementData
	{
	public:
		std::string MulticastIP;
		std::string ListenIP;
		int MulticastPort;
		int ListenPort;
		bool IsConfigSet;
		SOCKET MulticastSocketFD;
		SOCKET ListenSocketFD;
	public:
		MasterImplementData()
		{
			this->MulticastIP = "";
			this->ListenIP = "";
			this->MulticastPort = 0;
			this->ListenPort = 0;
			this->IsConfigSet = false;
		}
	};

	Master::MasterImplementData* const Master::implData = new Master::MasterImplementData();

	void Master::Run()
	{
		if (!Master::implData->IsConfigSet)
		{
			throw MasterLaunchFailedException("尚未配置此Master的参数");
		}

#if __TINYROS_ON_WINDOWS_PRIDEF__
		WSADATA lpWsaData;
		int wsaStartRet = WSAStartup(MAKEWORD(2, 2), &lpWsaData);
		if (wsaStartRet != 0)
		{
			throw MasterLaunchFailedException("加载Windows Socket API失败");
		}
#endif

		Master::implData->MulticastSocketFD = socket(AF_INET, SOCK_DGRAM, 0);
		Master::implData->ListenSocketFD = socket(AF_INET, SOCK_DGRAM, 0);
		if (Master::implData->MulticastSocketFD == INVALID_SOCKET ||
			Master::implData->ListenSocketFD == INVALID_SOCKET)
		{
			throw MasterLaunchFailedException("初始化套接字失败");
		}

		bool thereIsOther = Master::ExistOtherMaster();

		if (thereIsOther)
		{
			throw MasterAlreadyExistExcepiton();
		}



	}

	void Master::Exit()
	{

	}

	void Master::LoadConfig(const char* configPath)
	{
		Json::CharReaderBuilder readerBuilder;
		Json::Value config;
		std::unique_ptr<Json::CharReader> const reader(readerBuilder.newCharReader());

		std::ifstream fileIn(configPath, std::ios::binary);
		if (!fileIn.is_open())
		{
			throw std::exception();
		}
		std::stringstream configSS;
		std::string configJsonStr;
		std::string parseErr;
		configSS << fileIn.rdbuf();
		configJsonStr = configSS.str();

		bool succ = reader->parse(configJsonStr.c_str(), configJsonStr.c_str() + configJsonStr.length(), &config, &parseErr);

		if (succ)
		{
			Master::implData->MulticastIP = config["MulticastIP"].asString();
			Master::implData->ListenIP = config["ListenIP"].asString();
			Master::implData->MulticastPort = config["MulticastPort"].asInt();
			Master::implData->ListenPort = config["ListenPort"].asInt();
			Master::implData->IsConfigSet = true;
		}
		else
		{
			throw InvalidConfigException("不能解Json配置文件");
		}
	}

	void Master::SaveConfig(const char* configPath)
	{
		Json::Value config;
		config["MulticastIP"] = Master::implData->MulticastIP;
		config["ListenIP"] = Master::implData->ListenIP;
		config["MulticastPort"] = Master::implData->MulticastPort;
		config["ListenPort"] = Master::implData->ListenPort;

		Json::StreamWriterBuilder swBuilder;
		std::unique_ptr<Json::StreamWriter> const sWriter(swBuilder.newStreamWriter());

		std::ofstream fileToWrite(configPath, std::ios::binary);
		sWriter->write(config, &fileToWrite);
	}

	bool Master::ExistOtherMaster()
	{
		return false;
	}

	void Master::BroadcastThread()
	{
		while (true)
		{

		}
	}
}
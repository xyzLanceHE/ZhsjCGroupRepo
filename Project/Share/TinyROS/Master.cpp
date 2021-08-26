#include "Master.h"
#include "Messages.h"
#include "Exceptions.h"

#ifndef __TINYROS_PLATFORM__
	#error 无效的平台信息
#endif

#if __TINYROS_ON_LINUX_PRIDEF__
	#include <unistd.h>	
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <netinet/in.h>
	#include <ifaddrs.h>
	#include <errno.h>
	using SOCKET = int;
	#define INVALID_SOCKET -1
	#define CloseSocket close
	#define ErrorCode errno
	#define RECEIVE_TIMEOUT EAGAIN
#elif __TINYROS_ON_WINDOWS_PRIDEF__
	#include <WinSock2.h>
	#include <Iphlpapi.h>
	#include <WS2tcpip.h>
	#pragma comment(lib, "WS2_32.lib")
	#pragma comment(lib, "Iphlpapi.lib")
	#define CloseSocket closesocket
	#define ErrorCode WSAGetLastError()
	#define RECEIVE_TIMEOUT WSAETIMEDOUT
#endif

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <thread>
#include "JsonCpp/include/json.h"

namespace TinyROS
{
	class Master::MasterImplementData
	{
	public:
		std::string MulticastIP;
		std::string ListenIP;
		int MulticastPort;
		int ListenPort;
		int MulticastLocalBindPort;
		bool IsConfigSet;
		bool ExitFlag;
		bool IsRunning;
		SOCKET MulticastSocketFD;
		SOCKET ListenSocketFD;
		sockaddr_in MulticastAddr;
	public:
		MasterImplementData()
		{
			this->MulticastIP = "";
			this->ListenIP = "";
			this->MulticastPort = 0;
			this->ListenPort = 0;
			this->MulticastLocalBindPort = 0;
			this->IsConfigSet = false;
			this->ExitFlag = false;
			this->IsRunning = false;
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

		std::cout << "启动中...\n";
		bool thereIsOther = Master::ExistOtherMaster();
		if (thereIsOther)
		{
			throw MasterAlreadyExistExcepiton();
		}

		Master::SetUpSocket();

		std::thread listenThread = std::thread(Master::ListenThread);
		std::thread multicastThread = std::thread(Master::BroadcastThread);
		listenThread.detach();
		multicastThread.detach();
		//std::cout << "run to the end\n";

	}

	void Master::Exit()
	{
		if (!Master::implData->IsRunning)
		{
			return;
		}
		Master::implData->ExitFlag = true;
		CloseSocket(Master::implData->ListenSocketFD);
		CloseSocket(Master::implData->MulticastSocketFD);
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
			Master::implData->MulticastLocalBindPort = config["MulticastLocalBindPort"].asInt();
			Master::implData->IsConfigSet = true;
		}
		else
		{
			throw InvalidConfigException("不能解析Json配置文件");
		}
	}

	void Master::SaveConfig(const char* configPath)
	{
		if (!Master::implData->IsConfigSet)
		{
			return;
		}
		Json::Value config;
		config["MulticastIP"] = Master::implData->MulticastIP;
		config["ListenIP"] = Master::implData->ListenIP;
		config["MulticastPort"] = Master::implData->MulticastPort;
		config["ListenPort"] = Master::implData->ListenPort;
		config["MulticastLocalBindPort"] = Master::implData->MulticastLocalBindPort;

		Json::StreamWriterBuilder swBuilder;
		std::unique_ptr<Json::StreamWriter> const sWriter(swBuilder.newStreamWriter());

		std::ofstream fileToWrite(configPath, std::ios::binary);
		sWriter->write(config, &fileToWrite);
	}

	bool Master::ExistOtherMaster()
	{
		SOCKET tempSocketFD = socket(AF_INET, SOCK_DGRAM, 0);
		if (tempSocketFD == INVALID_SOCKET)
		{
			throw MasterLaunchFailedException("未能测试是否有其他Master: 创建socket失败");
		}

		int ret;
		int reuseAddr = 1;
		ret = setsockopt(tempSocketFD, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&reuseAddr), sizeof(reuseAddr));
		if (ret != 0)
		{
			CloseSocket(tempSocketFD);
			throw MasterLaunchFailedException("未能测试是否有其他Master: port复用失败");
		}
		 
		sockaddr_in multicastReceiverBindAddr;
		multicastReceiverBindAddr.sin_family = AF_INET;
		multicastReceiverBindAddr.sin_port = htons(Master::implData->MulticastPort);
		//inet_pton(AF_INET, Master::implData->ListenIP.c_str(), &multicastReceiverBindAddr.sin_addr.s_addr);
		multicastReceiverBindAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		ret = bind(tempSocketFD, reinterpret_cast<sockaddr*>(&multicastReceiverBindAddr), sizeof(sockaddr_in));
		if (ret != 0)
		{
			CloseSocket(tempSocketFD);
			throw MasterLaunchFailedException("未能测试是否有其他Master: bind失败");
		}
		
#if __TINYROS_ON_WINDOWS_PRIDEF__
		int timeout = 5000; //5s
#elif __TINYROS_ON_LINUX_PRIDEF__
		timeval timeout = { 5, 0 }; //5s
#else
#endif
		setsockopt(tempSocketFD, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&timeout), sizeof(timeout));
		if (ret != 0)
		{
			CloseSocket(tempSocketFD);
			throw MasterLaunchFailedException("未能测试是否有其他Master: 设置超时失败");
		}

		ip_mreq multicastOpiton;
		inet_pton(AF_INET, Master::implData->MulticastIP.c_str(), &multicastOpiton.imr_multiaddr.s_addr);
		inet_pton(AF_INET, Master::implData->ListenIP.c_str(), &multicastOpiton.imr_interface.s_addr);
		ret = setsockopt(tempSocketFD, IPPROTO_IP, IP_ADD_MEMBERSHIP,
			reinterpret_cast<char*>(&multicastOpiton), sizeof(multicastOpiton));
		if (ret != 0)
		{
			CloseSocket(tempSocketFD);
			throw MasterLaunchFailedException("未能测试是否有其他Master: 加入组播失败");
		}


		char recvBuf[100];
		int len = recvfrom(tempSocketFD, recvBuf, sizeof(recvBuf), 0, nullptr, nullptr);
		if (len != -1)
		{
			CloseSocket(tempSocketFD);
			return true;
		}
		else
		{
			int err = ErrorCode;
			std::cout << err << std::endl;
			CloseSocket(tempSocketFD);
			switch (err)
			{
			case RECEIVE_TIMEOUT:
				return false;
			default:
				return true;
			}
		}
	}

	void Master::SetUpSocket()
	{
		Master::implData->MulticastSocketFD = socket(AF_INET, SOCK_DGRAM, 0);
		Master::implData->ListenSocketFD = socket(AF_INET, SOCK_DGRAM, 0);
		if (Master::implData->MulticastSocketFD == INVALID_SOCKET ||
			Master::implData->ListenSocketFD == INVALID_SOCKET)
		{
			throw MasterLaunchFailedException("初始化套接字失败");
		}

		int ret;

		// 绑定收听端口
		// 该端口无需固定，因为MasterBroadcastDatagram携带此信息，从而Node可以得知Master的收听端口
		// 如果端口被占用，可以换一个
		sockaddr_in listenAddr;
		listenAddr.sin_family = AF_INET;
		listenAddr.sin_port = htons(Master::implData->ListenPort);
		inet_pton(AF_INET, Master::implData->ListenIP.c_str(), &listenAddr.sin_addr.s_addr);
		ret = bind(Master::implData->ListenSocketFD, 
			reinterpret_cast<sockaddr*>(&listenAddr), sizeof(listenAddr));
		if (ret != 0)
		{
			// TODO : 如果是端口被占用，自动尝试别的端口
			throw MasterLaunchFailedException("未能绑定收听套接字");
		}


		// 绑定广播发送套接字的本地端口
		// 该端口不重要，重要的是绑定到属于局域网的IP，因此和ListenIP是同一个IP，否则，可能广播到其他虚拟网络
		// 如果端口被占用，可以换一个
		sockaddr_in multicastSenderBindAddr;
		multicastSenderBindAddr.sin_family = AF_INET;
		multicastSenderBindAddr.sin_port = htons(Master::implData->MulticastLocalBindPort);
		inet_pton(AF_INET, Master::implData->ListenIP.c_str(), &multicastSenderBindAddr.sin_addr.s_addr);
		ret = bind(Master::implData->MulticastSocketFD, 
			reinterpret_cast<sockaddr*>(&multicastSenderBindAddr), sizeof(multicastSenderBindAddr));
		if (ret != 0)
		{
			// TODO : 如果是端口被占用，自动尝试别的端口
			throw MasterLaunchFailedException("未能绑定广播套接字");
		}

		// 设置广播发送的端口
		// 接收者需要绑定此端口才能收到广播，因此Node的设置应与此一致
		sockaddr_in& multicastAddr = Master::implData->MulticastAddr;
		multicastAddr.sin_family = AF_INET;
		multicastAddr.sin_port = htons(Master::implData->MulticastPort);
		inet_pton(AF_INET, Master::implData->MulticastIP.c_str(), &multicastAddr.sin_addr.s_addr);
	}

	void Master::BroadcastThread()
	{
		while (true)
		{
			if (Master::implData->ExitFlag)
			{
				break;
			}

		}
	}

	void Master::ListenThread()
	{
		while (true)
		{
			if (Master::implData->ExitFlag)
			{
				break;
			}

		}
	}
}
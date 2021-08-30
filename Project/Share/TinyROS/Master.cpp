#include "Master.h"
#include "Node.h"
#include "Messages.h"
#include "Exceptions.h"

#ifndef __TINYROS_PLATFORM__
	#error 无效的平台信息
#endif

#include "TinyROSNetPrefix.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <thread>
#include <chrono>
#include <csignal>
#include <map>
#include <mutex>
#include <set>
#include "LibWrapper.h"
//#include "OpenSSL/include/sha.h"
#include "JsonCpp/include/json.h"

namespace TinyROS
{
	using TopicSet = std::set<SHA256Value, SHA256ValueComparator>;
	
	struct NodeInformation
	{
		std::string Name;
		TopicSet Subscribed;
		TopicSet Published;
	};

	struct TopicInformation
	{
		TopicPort Port;
		std::string Name;
		TypeIDHash Type;
	};

	using NodeDictionary = std::map<SHA256Value, NodeInformation, SHA256ValueComparator>;
	using TopicDictionary = std::map<SHA256Value, TopicInformation, SHA256ValueComparator>;

	class Master::MasterImplementData
	{
	public:

		std::string MulticastIP;
		std::string ListenIP;
		int MulticastPort;
		int ListenPort;
		int MulticastLocalBindPort;
		bool IsConfigSet;
		int TopicStartPort;
		int NextTopicPort;

		int ExitFlag;
		bool IsRunning;
		bool IsColsed;
		bool KeyboardInterruptFlag;

		SOCKET MulticastSocketFD;
		SOCKET ListenSocketFD;
		sockaddr_in MulticastAddr;
		MasterBroadcastDatagram BroadcastMessage;

		NodeDictionary Nodes;
		TopicDictionary Topics;

		enum class NodeOperationType { Register, Unregister };
		enum class TopicOperationType { Publish, Subscribe };
		std::mutex NodesOperationMutex;
		std::mutex TopicsOperationMutex;
	public:
		MasterImplementData()
		{
			this->MulticastIP = "";
			this->ListenIP = "";
			this->MulticastPort = 0;
			this->ListenPort = 0;
			this->MulticastLocalBindPort = 0;
			this->TopicStartPort = 6670;
			this->IsConfigSet = false;
			this->ExitFlag = 0b00;
			this->IsRunning = false;
			this->IsColsed = false;
			this->KeyboardInterruptFlag = false;
			this->BroadcastMessage.Signal = 0;
			this->BroadcastMessage.ListenPort = 0;
		}

		void RequestTooLongHandler(sockaddr_in* srcAddr);
		void RequestHandler(char* buf, int len, sockaddr_in* srcAddr);
		void HandleNodeOperation(NodeOperationType type, SHA256Value receivedHash, std::string name, sockaddr_in* srcAddr);
		void HandleTopicOperation(TopicOperationType type, SHA256Value nodeHash, SHA256Value topicNameHash, std::string topicName, TypeIDHash hash, bool createIfNo, sockaddr_in* srcAddr);
	};

	Master::MasterImplementData* const Master::implData = new Master::MasterImplementData();

	void Master::Run()
	{
		if (Master::implData->IsColsed)
		{
			throw MasterLaunchFailedException("Master已关闭");
		}

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

		Master::implData->NextTopicPort = Master::implData->TopicStartPort;
		Master::implData->BroadcastMessage.Signal = 1;
		std::thread listenThread = std::thread(Master::ListenThread);
		std::thread multicastThread = std::thread(Master::BroadcastThread);
		listenThread.detach();
		multicastThread.detach();
		
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(0.2s);
		std::cout << "Master开始已运行\n";
		Master::implData->IsRunning = true;
	}

	void Master::Spin()
	{
		Master::TakeOverCtrlC();
		while (true)
		{
			if (Master::implData->KeyboardInterruptFlag)
			{
				break;
			}
		}
	}

	void Master::Exit()
	{
		if ((!Master::implData->IsRunning) || Master::implData->IsColsed)
		{
			std::cout << "Exit doing nothing\n";
			return;
		}
		Master::implData->BroadcastMessage.Signal = 2;
		Master::implData->ExitFlag = 0b11;
		std::cout << "Exitting\n";
		int leftPeriod = 10;
		while (Master::implData->ExitFlag != 0 && leftPeriod != 0)
		{
			leftPeriod--;
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(0.3s);
		}
		CloseSocket(Master::implData->ListenSocketFD);
		CloseSocket(Master::implData->MulticastSocketFD);
		std::cout << "resources closed, process exited\n";
		Master::implData->IsColsed = true;
	}

	void Master::LoadConfig(const char* configPath)
	{
		Json::CharReaderBuilder readerBuilder;
		Json::Value config;
		std::unique_ptr<Json::CharReader> const reader(readerBuilder.newCharReader());

		std::ifstream fileIn(configPath, std::ios::binary);
		if (!fileIn.is_open())
		{
			throw InvalidConfigException("未能加载配置文件，请检查文件路径");
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
			Master::implData->TopicStartPort = config["TopicStartPort"].asInt();
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
		config["TopicStartPort"] = Master::implData->TopicStartPort;

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
		ret = setsockopt(tempSocketFD, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&timeout), sizeof(timeout));
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
			CloseSocket(tempSocketFD);
			switch (err)
			{
			case RECEIVE_TIMEOUT:
				return false;
			default:
				std::cout << err << std::endl;
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
		Master::implData->BroadcastMessage.ListenPort = Master::implData->ListenPort;


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
		std::cout << "广播线程已启动\n";
		int exitBroadcastCount = 3;
		while (true)
		{
			int resetThis = Master::implData->ExitFlag & 0b01;
			if (resetThis)
			{
				exitBroadcastCount--;
				if (exitBroadcastCount == 0)
				{
					Master::implData->ExitFlag &= 0b10;
					break;
				}
			}
			
			int sentLen = sendto(Master::implData->MulticastSocketFD,
				reinterpret_cast<char*>(&Master::implData->BroadcastMessage),
				sizeof(MasterBroadcastDatagram), 0,
				reinterpret_cast<sockaddr*>(&Master::implData->MulticastAddr), sizeof(sockaddr_in));
			if (sentLen == -1)
			{
				std::cout << "广播时出现问题:" << ErrorCode << std::endl;
			}

			using namespace std::chrono_literals;
			std::this_thread::sleep_for(0.8s);
		}
	}

	void Master::ListenThread()
	{
		std::cout << "收听线程已启动\n";
		while (true)
		{
			int resetThis = Master::implData->ExitFlag & 0b10;
			if (resetThis)
			{
				Master::implData->ExitFlag &= 0b01;
				break;
			}

			char* buf = new char[128];
			sockaddr_in* nodeAddr = new sockaddr_in;
			socklen_t addrLen = sizeof(sockaddr_in);

			int rcvdLen = recvfrom(Master::implData->ListenSocketFD, buf, 128,
				0, reinterpret_cast<sockaddr*>(nodeAddr), &addrLen);
			if (rcvdLen == -1)
			{
				int err = ErrorCode;
				if (err == MESSAGE_TOO_LONG)
				{
					// 该线程负责回收nodeAddr
					std::thread handleTooLongMsg(&Master::MasterImplementData::RequestTooLongHandler, Master::implData, nodeAddr);
					handleTooLongMsg.detach();
				}
				delete[] buf;
			}
			else
			{
				// 该线程负责回收buf和nodeAddr
				std::thread handleLongMsg(&Master::MasterImplementData::RequestHandler, Master::implData, buf, rcvdLen, nodeAddr);
				handleLongMsg.detach();
			}
		}
	}

	void Master::TakeOverCtrlC()
	{
		Master::implData->KeyboardInterruptFlag = false;
		signal(SIGINT, Master::CtrlCHandler);
	}

	void Master::CtrlCHandler(int signal)
	{
		std::cout << "You've pressed ctrl + C...\n";
		if (!Master::implData->IsColsed)
		{
			Master::Exit();
			Master::implData->KeyboardInterruptFlag = true;
		}
		else
		{
			Master::implData->KeyboardInterruptFlag = true;
			exit(0);
		}
	}

	void Master::MasterImplementData::RequestTooLongHandler(sockaddr_in* srcAddr)
	{
		int replyMsg[2] = { RequestFail, RequestTooLong };
		int sentLen = sendto(this->ListenSocketFD, reinterpret_cast<char*>(replyMsg),
			sizeof(replyMsg), 0, reinterpret_cast<sockaddr*>(srcAddr), sizeof(sockaddr_in));
		if (sentLen == -1)
		{

		}
		delete srcAddr;
	}

	void Master::MasterImplementData::RequestHandler(char* buf, int len, sockaddr_in* srcAddr)
	{
		int* pRequestType = reinterpret_cast<int*>(buf);
		switch (*pRequestType)
		{
		case RequestRegister:
		{
			SHA256Value rcvdSHA;
			std::copy(buf + HeadLen, buf + HeadLen + HashLen, reinterpret_cast<char*>(&rcvdSHA));
			std::string name(buf + HeadLen + HashLen, len - HeadLen - HashLen);
			this->HandleNodeOperation(NodeOperationType::Register, rcvdSHA, name, srcAddr);
			break;
		}
		case RequestPublish:
		case RequestSubscribe:
		{
			SHA256Value nodeSHA;
			SHA256Value topicNameSHA;
			std::copy(buf + HeadLen, buf + HeadLen + HashLen, reinterpret_cast<char*>(&nodeSHA));
			std::copy(buf + HeadLen + HashLen, buf + HeadLen + 2 * HashLen, reinterpret_cast<char*>(&topicNameSHA));
			TypeIDHash type = *(reinterpret_cast<TypeIDHash*>(buf + HeadLen + 2 * HashLen));
			bool createIfNotExist = *(reinterpret_cast<bool*>(buf + HeadLen + 2 * HashLen + TopicTypeLen));
			constexpr int prefixLen = HeadLen + 2 * HashLen + TopicTypeLen + FlagLen;
			std::string topicName(buf + prefixLen, len - prefixLen);
			
			TopicOperationType opType = TopicOperationType::Publish;
			if (*pRequestType == RequestSubscribe)
			{
				opType = TopicOperationType::Subscribe;
			}
			this->HandleTopicOperation(opType, nodeSHA, topicNameSHA, topicName,
				type, createIfNotExist, srcAddr);
			break;
		}
		case RequestUnregister:
		{

			break;
		}
		default:
			break;
		}

		delete srcAddr;
		delete[] buf;
	}

	void Master::MasterImplementData::HandleNodeOperation(NodeOperationType type, SHA256Value receivedHash, std::string name, sockaddr_in* srcAddr)
	{
		switch (type)
		{
		case TinyROS::Master::MasterImplementData::NodeOperationType::Register:
		{
			SHA256Value calculatedHash;
			SetSHA256InPlace(name.c_str(), name.size(), &calculatedHash);
			int msg[2];
			if (calculatedHash != receivedHash)
			{
				msg[0] = RequestFail;
				msg[1] = RegisterBadCheck;
				std::cout << "Node注册的hash校验失败:" << name << std::endl;
			}
			else
			{
				this->NodesOperationMutex.lock();
				if (this->Nodes.find(calculatedHash) == this->Nodes.end())
				{
					this->Nodes[calculatedHash] = { name, TopicSet(), TopicSet() };
					msg[0] = RequestSuccess;
					msg[1] = RequestSuccess;
					std::cout << "Node注册成功:" << name << std::endl;
				}
				else
				{
					msg[0] = RequestFail;
					msg[1] = RegisterNameDuplicate;
					std::cout << "Node名称重复，注册失败:" << name << std::endl;
				}
				this->NodesOperationMutex.unlock();
			}
			int sentLen = sendto(this->ListenSocketFD, reinterpret_cast<char*>(msg), sizeof(msg), 0,
				reinterpret_cast<sockaddr*>(srcAddr), sizeof(sockaddr_in));
			if (sentLen == -1)
			{

			}
			break;
		}
		case TinyROS::Master::MasterImplementData::NodeOperationType::Unregister:
		{
			
			break;
		}
		default:
			break;
		}
	}

	void Master::MasterImplementData::HandleTopicOperation(TopicOperationType opType, SHA256Value nodeHash, SHA256Value topicNameHash, std::string topicName, TypeIDHash topicTypeHash, bool createIfNo, sockaddr_in* srcAddr)
	{
		SHA256Value calculatedHash;
		SetSHA256InPlace(topicName.c_str(), topicName.size(), &calculatedHash);

		int msg[3];
		if (calculatedHash != topicNameHash)
		{
			msg[0] = RequestFail;
			msg[1] = TopicNameBadCheck;
			std::cout << "topicID的hash校验失败:" << topicName << std::endl;
		}

		int port;

		this->TopicsOperationMutex.lock();
		if (this->Topics.find(topicNameHash) == this->Topics.end())
		{
			if (createIfNo)
			{
				std::cout << "收到新的话题请求，分配端口:" << this->NextTopicPort << std::endl;
				port = this->NextTopicPort;
				this->NextTopicPort++;
				TopicInformation info;
				info.Port = port;
				info.Name = topicName;
				info.Type = topicTypeHash;
				this->Topics[topicNameHash] = info;
				msg[0] = RequestSuccess;
				msg[1] = port;
				msg[2] = port;
			}
			else
			{
				msg[0] = RequestFail;
				msg[1] = TopicNotExist;
			}
		}
		else
		{
			if (this->Topics[topicNameHash].Type == topicTypeHash)
			{
				std::cout << "收到已有话题请求，端口:" << this->NextTopicPort << std::endl;
				port = this->Topics[topicNameHash].Port;
				msg[0] = RequestSuccess;
				msg[1] = port;
				msg[2] = port;
			}
			else
			{
				msg[0] = RequestFail;
				msg[1] = TopicTypeError;
			}
		}
		this->TopicsOperationMutex.unlock();

		this->NodesOperationMutex.lock();
		if (this->Nodes.find(nodeHash) == this->Nodes.end())
		{
			msg[0] = RequestFail;
			msg[1] = UnregisteredNode;
			// 如果刚才新增了话题，要不要撤销？
			// 懒得写了
		}
		else
		{
			switch (opType)
			{
			case TinyROS::Master::MasterImplementData::TopicOperationType::Publish:
				if (this->Nodes[nodeHash].Published.find(topicNameHash) == this->Nodes[nodeHash].Published.end())
				{
					this->Nodes[nodeHash].Published.insert(topicNameHash);
				}
				else
				{
					msg[0] = RequestFail;
					msg[1] = AlreadyPubToTopic;
				}
				break;
			case TinyROS::Master::MasterImplementData::TopicOperationType::Subscribe:
				if (this->Nodes[nodeHash].Subscribed.find(topicNameHash) == this->Nodes[nodeHash].Subscribed.end())
				{
					this->Nodes[nodeHash].Subscribed.insert(topicNameHash);
				}
				else
				{
					// 暂时允许重复订阅，为了更加方便注册多个回调函数。
					//msg[0] = RequestFail;
					//msg[1] = AlreadySubFromTopic;
				}
				break;
			default:
				break;
			}
		}
		this->NodesOperationMutex.unlock();
		int sentLen = sendto(this->ListenSocketFD, reinterpret_cast<char*>(msg), sizeof(msg), 0,
			reinterpret_cast<sockaddr*>(srcAddr), sizeof(sockaddr_in));
		if (sentLen == -1)
		{

		}	
	}
}
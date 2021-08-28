#include "Node.h"
#include "Messages.h"
#include "Exceptions.h"
#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>
#include <algorithm>
#include "OpenSSL/include/sha.h"

#ifndef __TINYROS_PLATFORM__
	#error 无效的平台信息
#endif

#include "TinyROSNetPrefix.h"

namespace TinyROS
{
    class Node::NodeImplementData
    {
    public:
        std::vector<std::string> IpStrList;
        std::string LocalIP;
        std::string BroadcastIP;
        SOCKET MasterReceiverSocketFD;
        SOCKET MasterTalkerSocketFD;
        std::string Name;
        SHA256Value NameHash;
        in_addr MasterListenAddr;
        int MasterListenPort;
        int BoradcastPort;
        int ResetFlag;
    public:
        static constexpr const char* DefaultBroadcastIP = "239.0.1.10";
        static constexpr int DefaultBroadcastPort = 6666;
    public:
        NodeImplementData()
        {
            
        }
    };

    Node::NodeImplementData* const Node::implData = new Node::NodeImplementData();
   
    bool Node::IsInitialized = false;

	void Node::Init(const char* name)
	{
        if (Node::IsInitialized)
        {
            throw NodeInitializeFailedException("Node已经初始化过");
        }

        Node::implData->Name = name;
        std::cout << Node::implData->Name << std::endl;
        Node::SetUpNameHash();

        Node::LoadIPList();
        if (Node::implData->IpStrList.size() == 0)
        {
            throw NodeInitializeFailedException("未能获取本机ip");
        }

#if __TINYROS_ON_WINDOWS_PRIDEF__
        WSADATA lpWsaData;
        int wsaStartRet = WSAStartup(MAKEWORD(2, 2), &lpWsaData);
        if (wsaStartRet != 0)
        {
            throw NodeInitializeFailedException("加载Windows Socket API失败");
        }
#endif

        Node::ScanForMaster();

        Node::implData->BoradcastPort = Node::NodeImplementData::DefaultBroadcastPort;
        Node::implData->BroadcastIP = Node::NodeImplementData::DefaultBroadcastIP;
        Node::SetUpSocket();

        Node::RegisterSelf();

        Node::IsInitialized = true;

        std::thread broadcastReceive(&Node::MasterReceiveThread);
        broadcastReceive.detach();
	}

    void Node::Init(const char* name, const char* configPath)
    {
        throw NodeInitializeFailedException("此方法尚未实现\n");
    }

    void Node::HashTest()
    {
        std::string msg("this is a node");
        SHA256Value hashVal;
        SHA256((unsigned char*)msg.c_str(), msg.size(), hashVal.value);
        std::cout << "sha256 of the message:" << hashVal.ToHexString().c_str() << std::endl;
    }

    void Node::SetUpSocket()
    {
        Node::implData->MasterTalkerSocketFD = socket(AF_INET, SOCK_DGRAM, 0);
        Node::implData->MasterReceiverSocketFD = socket(AF_INET, SOCK_DGRAM, 0);
        if (Node::implData->MasterTalkerSocketFD == INVALID_SOCKET ||
            Node::implData->MasterReceiverSocketFD == INVALID_SOCKET)
        {
            throw NodeInitializeFailedException("创建套接字失败");
        }

        int ret;

        // MasterTalkerSocketFD用于主动向Master通信
        // 不需要绑定， 具有接收超时限制
#if __TINYROS_ON_WINDOWS_PRIDEF__
        int timeout = 3000; //3s
#elif __TINYROS_ON_LINUX_PRIDEF__
        timeval timeout = { 3, 0 }; //3s
#else
#endif
        ret = setsockopt(implData->MasterTalkerSocketFD, SOL_SOCKET, SO_RCVTIMEO,
            reinterpret_cast<char*>(&timeout), sizeof(timeout));
        if (ret != 0)
        {
            throw NodeInitializeFailedException("超时设置失败");
        }

        // MasterReceiverSocketFD用于接收广播
        // 需要绑定到特定端口
        int reuseAddr = 1;
        ret = setsockopt(Node::implData->MasterReceiverSocketFD, SOL_SOCKET, SO_REUSEADDR,
            reinterpret_cast<char*>(&reuseAddr), sizeof(reuseAddr));
        if (ret != 0)
        {
            throw NodeInitializeFailedException("设置端口复用失败");
        }

        ret = setsockopt(implData->MasterReceiverSocketFD, SOL_SOCKET, SO_RCVTIMEO,
            reinterpret_cast<char*>(&timeout), sizeof(timeout));
        if (ret != 0)
        {
            throw NodeInitializeFailedException("超时设置失败");
        }

        sockaddr_in multicastAddr;
        multicastAddr.sin_family = AF_INET;
        multicastAddr.sin_port = htons(Node::implData->BoradcastPort);
        multicastAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        ret = bind(Node::implData->MasterReceiverSocketFD, reinterpret_cast<sockaddr*>(&multicastAddr), sizeof(multicastAddr));
        if (ret != 0)
        {
            throw NodeInitializeFailedException("广播收听套接字绑定失败");
        }

        ip_mreq multicastOption;
        inet_pton(AF_INET, Node::implData->BroadcastIP.c_str(), &multicastOption.imr_multiaddr);
        inet_pton(AF_INET, Node::implData->LocalIP.c_str(), &multicastOption.imr_interface);
        ret = setsockopt(Node::implData->MasterReceiverSocketFD, IPPROTO_IP, IP_ADD_MEMBERSHIP,
            reinterpret_cast<char*>(&multicastOption), sizeof(multicastOption));
        if (ret != 0)
        {
            std::cout << "multicast join error:" << ErrorCode;
            throw NodeInitializeFailedException("加入广播组失败");
        }
    }

    void Node::SetUpNameHash()
    {
        SHA256(
            reinterpret_cast<const unsigned char*>(Node::implData->Name.c_str()),
            Node::implData->Name.size(),
            Node::implData->NameHash.value);
    }

    void Node::ScanForMaster()
    {
        bool foundInvalidMaster = false;
        std::cout << "开始尝试以下ip:\n";
        for (auto ipStr : Node::implData->IpStrList)
        {
            std::cout << ipStr << std::endl;
        }
        for (auto ipStr : Node::implData->IpStrList)
        {
            SOCKET tempSocket = socket(AF_INET, SOCK_DGRAM, 0);
            if (tempSocket == INVALID_SOCKET)
            {
                continue;
            }

            int ret;

            int reuseAddr = 1;
            ret = setsockopt(tempSocket, SOL_SOCKET, SO_REUSEADDR, 
                reinterpret_cast<char*>(&reuseAddr), sizeof(reuseAddr));
            if (ret != 0)
            {
                CloseSocket(tempSocket);
                continue;
            }

            sockaddr_in multicastAddr;
            multicastAddr.sin_family = AF_INET;
            multicastAddr.sin_port = htons(Node::NodeImplementData::DefaultBroadcastPort);
            //inet_pton(AF_INET, ipStr.c_str(), &multicastAddr.sin_addr.s_addr);
            multicastAddr.sin_addr.s_addr = htonl(INADDR_ANY);
            ret = bind(tempSocket, reinterpret_cast<sockaddr*>(&multicastAddr), sizeof(multicastAddr));
            if (ret != 0)
            {
                CloseSocket(tempSocket);
                continue;
            }

            ip_mreq multicastOption;
            inet_pton(AF_INET, Node::NodeImplementData::DefaultBroadcastIP, &multicastOption.imr_multiaddr);
            inet_pton(AF_INET, ipStr.c_str(), &multicastOption.imr_interface);
            ret = setsockopt(tempSocket, IPPROTO_IP, IP_ADD_MEMBERSHIP,
                reinterpret_cast<char*>(&multicastOption), sizeof(multicastOption));
            if (ret != 0)
            {
                CloseSocket(tempSocket);
                continue;
            }

#if __TINYROS_ON_WINDOWS_PRIDEF__
            int timeout = 3000; //3s
#elif __TINYROS_ON_LINUX_PRIDEF__
            timeval timeout = { 3, 0 }; //3s
#else
#endif
            ret = setsockopt(tempSocket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<char*>(&timeout), sizeof(timeout));
            if (ret != 0)
            {
                CloseSocket(tempSocket);
                continue;
            }

            char buf[sizeof(MasterBroadcastDatagram)];
            sockaddr_in masterAddr;
            masterAddr.sin_addr.s_addr = INADDR_ANY;
            masterAddr.sin_port = 0;
            masterAddr.sin_family = AF_INET;
            socklen_t addrLen = sizeof(sockaddr_in);
            int rcvdLen = recvfrom(tempSocket, buf, sizeof(MasterBroadcastDatagram), 0, 
                reinterpret_cast<sockaddr*>(&masterAddr), &addrLen);
            if (rcvdLen != sizeof(MasterBroadcastDatagram))
            {
                CloseSocket(tempSocket);
                continue;
            }
            else
            {
                MasterBroadcastDatagram* masterData = reinterpret_cast<MasterBroadcastDatagram*>(&buf);
                if (masterData->Signal == 1)
                {
                    Node::implData->MasterListenPort = masterData->ListenPort;
                    char masterIpBuf[40] = { 0 };
                    inet_ntop(AF_INET, &masterAddr.sin_addr.s_addr, 
                        masterIpBuf, sizeof(masterIpBuf));
                    std::string masterIpStr(masterIpBuf);
                    Node::implData->MasterListenAddr = masterAddr.sin_addr;
                    Node::implData->LocalIP = ipStr;
                    std::cout << "扫描到位于" << masterIpStr << ":" << Node::implData->MasterListenPort << "的Master\n";
                    std::cout << "使用本地的" << ipStr << "收听\n";
                    CloseSocket(tempSocket);
                    foundInvalidMaster = true;
                    break;
                }
                else
                {
                    CloseSocket(tempSocket);
                    continue;
                }
            }
        }
        if (!foundInvalidMaster)
        {
            throw NodeInitializeFailedException("Master not found");
        }
    }

    void Node::LoadIPList()
    {
#if __TINYROS_ON_WINDOWS_PRIDEF__
        PIP_ADAPTER_INFO pIpAdapterInfo;
        PIP_ADAPTER_INFO pInfoListHead = new IP_ADAPTER_INFO();
        unsigned char* infoDataBuffer = nullptr;
        unsigned long infoSize = sizeof(IP_ADAPTER_INFO);
        int ret = GetAdaptersInfo(pInfoListHead, &infoSize);
        int netCardNum = 0;
        if (ret == ERROR_BUFFER_OVERFLOW)
        {
            delete pInfoListHead;
            infoDataBuffer = new unsigned char[infoSize];
            pInfoListHead = reinterpret_cast<PIP_ADAPTER_INFO>(infoDataBuffer);
            ret = GetAdaptersInfo(pInfoListHead, &infoSize);
        }
        if (ret != 0)
        {
            throw NodeInitializeFailedException("未能获取本机ip");
        }
        pIpAdapterInfo = pInfoListHead;
        while (pIpAdapterInfo != nullptr)
        {
            IP_ADDR_STRING* pIpAddrString = &(pIpAdapterInfo->IpAddressList);
            while (pIpAddrString != nullptr)
            {
                std::string ipString(pIpAddrString->IpAddress.String);
                // std::cout << "IP:" << ipString << std::endl;
                if (ipString != "0.0.0.0")
                {
                    Node::implData->IpStrList.push_back(ipString);
                }
                pIpAddrString = pIpAddrString->Next;
            }
            pIpAdapterInfo = pIpAdapterInfo->Next;
        }
        if (infoDataBuffer != nullptr)
        {
            delete[] infoDataBuffer;
        }
        else
        {
            delete pInfoListHead; //虽然叫List，但这时候是一个单独的变量，不要用delete[]
        }
#elif __TINYROS_ON_LINUX_PRIDEF__
        ifaddrs* pIfAddrList = nullptr;
        ifaddrs* pIfAddrInfo = nullptr;
        getifaddrs(&pIfAddrList);
        pIfAddrInfo = pIfAddrList;
        while (pIfAddrInfo != nullptr)
        {
            sockaddr_in* pAddr = reinterpret_cast<sockaddr_in*>(pIfAddrInfo->ifa_addr);
            if (pAddr == nullptr)
            {
                pIfAddrInfo = pIfAddrInfo->ifa_next;
                continue;
            }
            if (pAddr->sin_family != AF_INET)
            {
                pIfAddrInfo = pIfAddrInfo->ifa_next;
                continue;
            }
            char ipAddressStrBuf[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(pAddr->sin_addr.s_addr), ipAddressStrBuf, INET_ADDRSTRLEN);
            std::string ipString(ipAddressStrBuf);
            // std::cout << "IP:" << ipString << std::endl;
            if (ipString != "0.0.0.0")
            {
                Node::implData->IpStrList.push_back(ipString);
            }
            pIfAddrInfo = pIfAddrInfo->ifa_next;
        }
        if (pIfAddrList != nullptr)
        {
            freeifaddrs(pIfAddrList);
        }
#else
        throw NodeInitializeFailedException();
#endif
    }

    void Node::RegisterSelf()
    {
        int nameLen = Node::implData->Name.size();
        int totalLen = nameLen + HeadLen + HashLen;
        char* registerMsgBuf = new char[totalLen];
        int* pHead = reinterpret_cast<int*>(registerMsgBuf);
        *pHead = RequestRegister;
        std::copy(Node::implData->NameHash.value, Node::implData->NameHash.value + HashLen, 
            reinterpret_cast<unsigned char*>(registerMsgBuf + HeadLen));
        std::copy(Node::implData->Name.begin(), Node::implData->Name.end(), 
            reinterpret_cast<unsigned char*>(registerMsgBuf + HeadLen + HashLen));

        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(Node::implData->MasterListenPort);
        addr.sin_addr = Node::implData->MasterListenAddr;
        int sentLen = sendto(Node::implData->MasterTalkerSocketFD, registerMsgBuf, totalLen,
            0, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
        delete[] registerMsgBuf;

        char* replyBuf = new char[32];
        int recvdLen = recvfrom(Node::implData->MasterTalkerSocketFD, replyBuf,
            32, 0, nullptr, nullptr);
        if (sentLen == -1 || recvdLen == -1)
        {
            delete[] replyBuf;
            throw NodeInitializeFailedException("未能向Master注册自己，可能是网络不佳");
        }

        int* pIntBuf = reinterpret_cast<int*>(replyBuf);
        if (pIntBuf[0] == RequestSuccess)
        {
            std::cout << "注册成功\n";
        }
        else if (pIntBuf[0] == RequestFail)
        {
            switch (pIntBuf[1])
            {
            case RequestTooLong:
                delete[] replyBuf;
                throw NodeInitializeFailedException("节点名称太长，注册失败");
                break;
            case RegisterNameDuplicate:
                delete[] replyBuf;
                throw NodeInitializeFailedException("节点名称重复，注册失败");
                break;
            case RegisterBadCheck:
                delete[] replyBuf;
                throw NodeInitializeFailedException("注册信息校验失败，可能是网络不佳");
                break;
            default:
                delete[] replyBuf;
                throw NodeInitializeFailedException("未能向Master注册自己，可能是网络不佳");
                break;
            }
        }
        else
        {
            delete[] replyBuf;
            throw NodeInitializeFailedException("未能向Master注册自己，未知的错误");
        }
    }

    void Node::UnregisterSelf()
    {

    }

    void Node::MasterReceiveThread()
    {
        int recvTimeoutCount = 0;
        while (true)
        {
            if (Node::implData->ResetFlag)
            {
                break;
            }

            char buf[sizeof(MasterBroadcastDatagram)];
            int rcvdLen = recvfrom(Node::implData->MasterReceiverSocketFD, buf, 
                sizeof(buf), 0, nullptr, nullptr);
            if (rcvdLen == -1)
            {
                int errCode = ErrorCode;
                std::cout << "broadcast recv error:" << errCode << std::endl;
                if (errCode == RECEIVE_TIMEOUT)
                {
                    recvTimeoutCount++;
                    if (recvTimeoutCount > 5)
                    {
                        std::cout << "No hearing from Master for a long time\n";
                        break;
                    }
                }
            }
            else
            {
                MasterBroadcastDatagram* data = reinterpret_cast<MasterBroadcastDatagram*>(buf);
                int signal = data->Signal;
                if (signal == 2)
                {
                    std::cout << "Master signal shutdown\n";
                }
                else if (signal == 1)
                {
                    std::cout << "Master signal running\n";
                }
            }
        }
    }

    TopicPort NodeInnerMethods::RequestTopic(const char* topicName, int requestType, TypeIDHash topicType, bool createIfNotExist)
    {
        if (!Node::IsInitialized)
        {
            throw NodeException("Node未初始化，无法进行此项操作");
        }

        std::string name(topicName);
        int nameLen = name.size();
        constexpr int prefixLen = HeadLen + 2 * HashLen + TopicTypeLen + FlagLen;
        int totalLen = prefixLen + nameLen;

        char* msgBuf = new char[totalLen];
        int* pHead = reinterpret_cast<int*>(msgBuf);
        *pHead = requestType;
        std::copy(reinterpret_cast<char*>(Node::implData->NameHash.value),
            reinterpret_cast<char*>(Node::implData->NameHash.value) + HashLen,
            msgBuf + HeadLen);
        SHA256Value topicNameHash;
        SHA256(reinterpret_cast<const unsigned char*>(name.c_str()), nameLen,
            topicNameHash.value);
        std::copy(reinterpret_cast<char*>(topicNameHash.value),
            reinterpret_cast<char*>(topicNameHash.value) + HashLen,
            msgBuf + HeadLen + HashLen);
        TypeIDHash* pTypeID = reinterpret_cast<TypeIDHash*>(msgBuf + HeadLen + 2 * HashLen);
        *pTypeID = topicType;
        bool* pFlag = reinterpret_cast<bool*>(msgBuf + HeadLen + 2 * HashLen + TopicTypeLen);
        *pFlag = createIfNotExist;
        std::copy(name.begin(), name.end(), msgBuf + prefixLen);

        sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(Node::implData->MasterListenPort);
        addr.sin_addr = Node::implData->MasterListenAddr;
        int sentLen = sendto(Node::implData->MasterTalkerSocketFD, msgBuf, totalLen,
            0, reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
        delete[] msgBuf;

        if (sentLen == -1)
        {
            throw TopicException("未能发出话题请求，可能是网络不佳");
        }

        int recvBuf[8];
        int rcvdLen = recvfrom(Node::implData->MasterTalkerSocketFD, reinterpret_cast<char*>(recvBuf), sizeof(recvBuf),
            0, nullptr, nullptr);
        if (rcvdLen == -1)
        {
            throw TopicException("未能收到话题请求的响应，可能是网络不佳");
        }

        if (recvBuf[0] == RequestSuccess)
        {
            if (recvBuf[1] == recvBuf[2]) // Master会把一个值返回两遍，用于简单校验
            {
                std::cout << "成功获取话题\n";
                return recvBuf[1];
            }
            else
            {
                throw TopicException("TopicID校验失败");
            }
        }
        else if(recvBuf[0] == RequestFail)
        {
            switch (recvBuf[1])
            {
            case TopicNameBadCheck:
                throw TopicException("请求信息校验失败，可能是网络不佳");
                break;
            case TopicNotExist:
                throw TopicException("要请求的话题不存在");
                break;
            case TopicTypeError:
                throw TopicException("申请的话题类型不匹配");
                break;
            case UnregisteredNode:
                throw TopicException("节点未注册，话题申请失败");
                break;
            default:
                throw TopicException("未能申请到话题，未知的错误");
                break;
            }
        }
        else
        {
            throw TopicException("未能申请到话题，未知的错误");
        }
        return -1;
    }

    const char* NodeInnerMethods::GetBroadcastIP()
    {
        return Node::implData->BroadcastIP.c_str();
    }

    const char* NodeInnerMethods::GetLocalIP()
    {
        return Node::implData->LocalIP.c_str();
    }


}
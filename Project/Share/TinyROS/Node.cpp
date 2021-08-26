#include "Node.h"
#include "Exceptions.h"
#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include "OpenSSL/include/sha.h"

#ifndef __TINYROS_PLATFORM__
	#error 无效的平台信息
#endif

#if __TINYROS_ON_LINUX_PRIDEF__
    #include <unistd.h>	
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
    class Node::NodeImplementData
    {
    public:
        std::vector<std::string> IpStrList;
        SOCKET MasterReceiverSocketFD;
        std::string Name;
    public:
        NodeImplementData()
        {
            
        }
    };

    Node::NodeImplementData* const Node::implData = new Node::NodeImplementData();
   
    bool Node::IsInitialized = false;

	void Node::HelloWorld()
	{
		printf("Hello world, TinyROS!\n");
	}

	void Node::Init(const char* name)
	{
        Node::implData->Name = name;
        Node::LoadIPList();
        if (Node::implData->IpStrList.size() == 0)
        {
            throw NodeInitializeFailedException("未能获取本机ip");
        }
        for (auto s : Node::implData->IpStrList)
        {
            std::cout << s << std::endl;
        }
        Node::ScanForMaster();
	}

    void Node::ScanForMaster()
    {
        throw NodeInitializeFailedException("没写完");
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

}
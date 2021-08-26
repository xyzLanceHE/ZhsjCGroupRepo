#pragma once
#include "TinyROSPlatformDef.h"

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
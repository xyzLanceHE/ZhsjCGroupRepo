#include <iostream>
#include "TinyROS/TinyROS.h"
#include <thread>
#include "MapTools.h"

int flag = 0;


void ChangeMapFlag(TinyROS::StringMessage msg)
{
	flag = 1;
}

int main()
{
	RoboTax::MapMessage map;
	DecodeFromPng("map.png", map);
	//节点初始化
	try
	{
		TinyROS::Node::Init("MapNode");
	}
	catch (TinyROS::TinyROSException& e)
	{
		std::cout << e.what();
		return -1;
	}
	//定义节点
	TinyROS::Publisher* mapPublisher;
	TinyROS::Subscriber* mapSubscriber;
	try
	{
		mapPublisher = TinyROS::NewPublisher<RoboTax::MapMessage>("localMap");
	}
	catch (TinyROS::TinyROSException& e)
	{
		std::cout << e.what();
		return -1;
	}

	TinyROS::MessageCallback<TinyROS::StringMessage> callback(1);
	callback.Register(ChangeMapFlag);
	try
	{
		mapSubscriber = TinyROS::NewSubscriber<TinyROS::StringMessage>("MapOrder", callback);
	}
	catch (TinyROS::TinyROSException& e)
	{
		std::cout << e.what();
		return -1;
	}
	while (true)
	{
		if (flag == 1)
		{
			mapPublisher->Publish(map);
			flag = 0;
		}

	}

	TinyROS::Node::Close();
}
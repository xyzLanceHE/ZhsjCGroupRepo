#include <iostream>
#include "TinyROS/TinyROS.h"
#include <thread>

int flag = 0;
struct map
{
	int localMap[400] = { 0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
						 0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
						 0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
						 0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
						 0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,
						 0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,
						 0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0 };
};

void ChangeMapFlag(TinyROS::StringMessage msg)
{
	flag = 1;
}

//int main()
//{
//	map rawMap;
//	TinyROS::SimpleObjectMessage<map> msg(rawMap);
//	//节点初始化
//	try
//	{
//		TinyROS::Node::Init("MapNode");
//	}
//	catch (TinyROS::TinyROSException& e)
//	{
//		std::cout << e.what();
//		return -1;
//	}
//	//定义节点
//	TinyROS::Publisher* mapPublisher;
//	TinyROS::Subscriber* mapSubscriber;
//	try
//	{
//		mapPublisher = TinyROS::NewPublisher<TinyROS::SimpleObjectMessage<map>>("localMap");
//	}
//	catch (TinyROS::TinyROSException& e)
//	{
//		std::cout << e.what();
//		return -1;
//	}
//
//	TinyROS::MessageCallback<TinyROS::StringMessage> callback(1);
//	callback.Register(ChangeMapFlag);
//	try
//	{
//		mapSubscriber = TinyROS::NewSubscriber<TinyROS::StringMessage>("StartOrder", callback);
//	}
//	catch (TinyROS::TinyROSException& e)
//	{
//		std::cout << e.what();
//		return -1;
//	}
//	while (true)
//	{
//		if (flag == 1)
//		{
//			mapPublisher->Publish(msg);
//			break;
//		}
//
//	}
//
//	TinyROS::Node::Close();
//}
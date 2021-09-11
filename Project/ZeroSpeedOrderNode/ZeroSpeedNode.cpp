#include <iostream>
#include "TinyROS/TinyROS.h"
#include "TinyROS/SharedMessageTypes.h"
#include <thread>

int main()
{
	//节点初始化
	try
	{
		TinyROS::Node::Init("ConstSpeedNode");
	}
	catch (TinyROS::TinyROSException& e)
	{
		std::cout << e.what();
		return -1;
	}

	//定义节点
	TinyROS::Publisher* ConstSpeedPub;
	try
	{
		ConstSpeedPub = TinyROS::NewPublisher<TinyROS::StringMessage>("ConstSpeedOrder");
	}
	catch (TinyROS::TinyROSException& e)
	{
		std::cout << e.what();
		return -1;
	}
	TinyROS::StringMessage msg("ConstSpeed");

	while (true)
	{
		ConstSpeedPub->Publish(msg);

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(10s);
	}
	TinyROS::Node::Close();
}
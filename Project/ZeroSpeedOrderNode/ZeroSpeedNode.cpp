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
		ConstSpeedPub = TinyROS::NewPublisher<RoboTax::CarVelocityMessage>("ZeroSpeedOrder");
	}
	catch (TinyROS::TinyROSException& e)
	{
		std::cout << e.what();
		return -1;
	}
	RoboTax::CarVelocity moveOrder;
	moveOrder.Linear = 0;
	moveOrder.Radius = INFINITY;
	RoboTax::CarVelocityMessage msg(moveOrder);

	while (true)
	{
		ConstSpeedPub->Publish(msg);
		std::cout << "send a zero speed msg" << std::endl;
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(0.5s);
	}
	TinyROS::Node::Close();
}
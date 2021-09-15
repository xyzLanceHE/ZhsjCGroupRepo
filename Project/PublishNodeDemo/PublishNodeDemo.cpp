#include <iostream>
#include "TinyROS/TinyROS.h"
#include "TinyROS/SharedMessageTypes.h"
#include <thread>





int main()
{

	//节点初始化
	try
	{
		TinyROS::Node::Init("TestNode");
	}
	catch (TinyROS::TinyROSException& e)
	{
		std::cout << e.what();
		return -1;
	}
	//定义节点
	TinyROS::Publisher* pForPub;
	TinyROS::Subscriber* pForSub;
	try
	{
		pForPub = TinyROS::NewPublisher<RoboTax::CarVelocityMessage>("MoveOrder");
	}
	catch (TinyROS::TinyROSException& e)
	{
		std::cout << e.what();
		return -1;
	}

	RoboTax::CarVelocity moveOrder;
	moveOrder.Linear = 0.1;
	moveOrder.Radius = 110.0;
	RoboTax::CarVelocityMessage msg(moveOrder);
	while (true)
	{
		char c;

		std::cin >> c;
		switch (c)
		{
		case 'w':
			msg.Value.Linear = 0.1;
			msg.Value.Radius = 1000;
			break;
		case 'a':
			msg.Value.Linear = 0.1;
			msg.Value.Radius = 0.0001;
			break;

		case 'd':
			msg.Value.Linear = 0.1;
			msg.Value.Radius = -0.0001;
			break;

		case 's':
			msg.Value.Linear = 0.0;
			msg.Value.Radius = 0.0;
			break;
		default:
			break;
		}
		pForPub->Publish(msg);

		std::cout << "send a move order"<<std::endl;

	}
	TinyROS::Node::Close();

}
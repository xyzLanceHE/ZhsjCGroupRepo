#include <iostream>
#include "TinyROS/TinyROS.h"
#include "TinyROS/SharedMessageTypes.h"
#include <thread>


int main()
{
	struct move
	{
		int rank;
		float linear;
		float radius;
	};
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
		ConstSpeedPub = TinyROS::NewPublisher<TinyROS::SimpleObjectMessage<move>>("MoveOrder");
	}
	catch (TinyROS::TinyROSException& e)
	{
		std::cout << e.what();
		return -1;
	}
	move moveOrder;
	moveOrder.rank = 3;
	moveOrder.linear = 0;
	moveOrder.radius = INFINITY;
	TinyROS::SimpleObjectMessage<move> msg(moveOrder);

	while (true)
	{
		ConstSpeedPub->Publish(msg);
		std::cout << "send a const msg" << std::endl;
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(0.5s);
	}
	TinyROS::Node::Close();
}
#include <iostream>
#include "MessageFilter.cpp"

int main()
{
	//初始化节点
	try
	{
		TinyROS::Node::Init("OrderFilterNode");
	}
	catch (TinyROS::TinyROSException& e)
	{
		std::cout << e.what();
		return -1;
	}

	TinyROS::Publisher* filtedOrderPub = nullptr;
	TinyROS::Subscriber* moveOrderSub;

	try
	{
		filtedOrderPub = TinyROS::NewPublisher<RoboTax::CarVelocityMessage>("cmd/vel/Noelle");
	}
	catch (TinyROS::TinyROSException& e)
	{
		std::cout << e.what();
	}

	RoboTax::MessageFilter<RoboTax::CarVelocityMessage> filter(filtedOrderPub);

	TinyROS::MessageCallback<RoboTax::CarVelocityMessage> moveOrderSubsciberCallback(1);

	RoboTax::PriorityVelocityCallback priority5Callback(&filter, 5);

	moveOrderSubsciberCallback.Register(priority5Callback);
	try
	{
		moveOrderSub = TinyROS::NewSubscriber<RoboTax::CarVelocityMessage>("MoveOrder", moveOrderSubsciberCallback);
	}
	catch (TinyROS::TinyROSException& e)
	{
		std::cout << e.what();
		return -1;
	}
    while (true)
    {
	
        //using namespace std::chrono_literals;
        //std::this_thread::sleep_for(1s);
    }

	TinyROS::Node::Close();
}
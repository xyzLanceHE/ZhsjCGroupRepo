#include <iostream>
#include "TinyROS/TinyROS.h"
#include <thread>

int constSpeedFlag = 0, manualSpeedFlag = 0, routeSpeedFlag = 0;
struct move
{
	float linear;
	float radius;
};
move constMoveOrder, manualMoveOrder, routeMoveOrder;

//收到命令则将其相应的flag置1
void ConstSpeedOrderCallback(TinyROS::StringMessage msg)
{
	constSpeedFlag = 1;
	constMoveOrder.linear = 0.0;
	constMoveOrder.radius = INFINITY;
}

void ManualSpeedOrderCallback(TinyROS::StringMessage msg)
{
    manualSpeedFlag = 1;
	manualMoveOrder.linear = 1.0;
	manualMoveOrder.radius = INFINITY;
}

void RouteSpeedOrderCallback(TinyROS::SimpleObjectMessage<move> msg)
{
    routeSpeedFlag = 1;
	std::cout << "linear:" << msg.Value.linear << " " << "radius:" << msg.Value.radius << std::endl;
	routeMoveOrder.linear = msg.Value.linear;
	routeMoveOrder.radius = msg.Value.radius;
}

//设定优先级，选择当前有的优先级高的命令进行发送
int CheckFlag()
{
	if (manualSpeedFlag == 1)
	{
		return 1;
	}
	else if (routeSpeedFlag == 1)
	{
		return 2;
	}
	else if (constSpeedFlag == 1)
	{
		return 3;
	}
}

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

	//发布一个命令，设一个publisher，收听三个命令，设3个subscriber
    TinyROS::Publisher* filtedOrderPub;
	TinyROS::Subscriber* constSpeedOrderSub;
	TinyROS::Subscriber* manualSpeedOrderSub;
	TinyROS::Subscriber* routeSpeedOrderSub;

	//发布的主题为FiltedOrder
    try
    {
		filtedOrderPub = TinyROS::NewPublisher<TinyROS::SimpleObjectMessage<int>>("FiltedOrder");
    }
    catch (TinyROS::TinyROSException& e)
    {
        std::cout << e.what();
        return -1;
    }

	//注册相应的回调函数
	TinyROS::MessageCallback<TinyROS::StringMessage> constCallback(1);
	constCallback.Register(ConstSpeedOrderCallback);

	TinyROS::MessageCallback<TinyROS::StringMessage> manualCallback(1);
	manualCallback.Register(ManualSpeedOrderCallback);

	TinyROS::MessageCallback<TinyROS::SimpleObjectMessage<move>> routeCallback(1);
	routeCallback.Register(RouteSpeedOrderCallback);
	//收听不同的命令
	try
	{
		constSpeedOrderSub = TinyROS::NewSubscriber<TinyROS::StringMessage>("ConstSpeedOrder", constCallback);
	}
	catch (TinyROS::TinyROSException& e)
	{
		std::cout << e.what();
		return -1;
	}
	try
	{
		manualSpeedOrderSub = TinyROS::NewSubscriber<TinyROS::StringMessage>("ManualSpeedOrder", manualCallback);
	}
	catch (TinyROS::TinyROSException& e)
	{
		std::cout << e.what();
		return -1;
	}
	try
	{
		manualSpeedOrderSub = TinyROS::NewSubscriber<TinyROS::SimpleObjectMessage<move>>("RouteSpeedOrder", routeCallback);
	}
	catch (TinyROS::TinyROSException& e)
	{
		std::cout << e.what();
		return -1;
	}
	int flagNow;
    while (true)
    {
		//检查当前最优先的命令是谁，并将其发送出去
		flagNow = CheckFlag();
		if (flagNow == 1)
		{
			TinyROS::SimpleObjectMessage<move> msg1(manualMoveOrder);
			filtedOrderPub->Publish(msg1);
		}
		else if (flagNow == 2)
		{
			TinyROS::SimpleObjectMessage<move> msg2(routeMoveOrder);
			filtedOrderPub->Publish(msg2);
		}
		else if (flagNow == 3)
		{
			TinyROS::SimpleObjectMessage<move> msg3(constMoveOrder);
			filtedOrderPub->Publish(msg3);
		}

		//将标识位重新置零
		if (flagNow == 1)
		{
			manualSpeedFlag = 0;
		}
		else if (flagNow == 2)
		{
			routeSpeedFlag = 0;
		}
		else if (flagNow == 3)
		{
			constSpeedFlag = 0;
		}

		//线程挂起2s
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
    }

	TinyROS::Node::Close();
}
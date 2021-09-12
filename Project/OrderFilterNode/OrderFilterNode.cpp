#include <iostream>
#include "TinyROS/TinyROS.h"
#include <thread>
#include <ctime>
#include <map> 

struct move
{
	int rank;
	float linear;
	float radius;
};

class PriorityClass
{
public:
	TinyROS::Publisher* filtedOrderPub;
	std::map<int, time_t> rankMap;
	PriorityClass(TinyROS::Publisher* filtedOrderPub)
	{
		this->filtedOrderPub = filtedOrderPub;
	}
	
	void CallbackInObject(TinyROS::SimpleObjectMessage<move> msg)
	{
		if (this->rankMap.count(msg.Value.rank) == 0)
		{
			this->rankMap[msg.Value.rank] = time(NULL);
		}
		time_t timeNow;
		if (msg.Value.rank >= 3)
		{
			this->rankMap[msg.Value.rank] = time(NULL);
		}
		std::map<int, time_t>::iterator it = this->rankMap.begin();
		while (it != this->rankMap.end())
		{
			timeNow = time(NULL);
			//有优先级较高的未过期的命令,刷新命令时间，不刷新操作,直接退出检查
			if (it->first < msg.Value.rank && timeNow - it->second <= 2)
			{
				this->rankMap[msg.Value.rank] = time(NULL);
				break;
			}
			//有优先级较高的命令但已过期，不作操作，继续检查下一优先级
			else if (it->first < msg.Value.rank && timeNow - it->second > 2)
			{
				++it;
				continue;
			}
			//没有比自己优先级高且有效的,刷新当前rank的时间并执行
			else if (it->first == msg.Value.rank)
			{
				this->rankMap[msg.Value.rank] = time(NULL);
				this->filtedOrderPub->Publish(msg);
				std::cout << "linear:" << msg.Value.linear << ", radius" << msg.Value.radius << std::endl;
				std::map<int, time_t>::iterator it1 = this->rankMap.begin();
				while (it1 != this->rankMap.end())
				{
					std::cout << it1->first << std::endl;
					++it1;
				}
				break;
			}
		}
	}
};


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
		filtedOrderPub = TinyROS::NewPublisher<TinyROS::SimpleObjectMessage<move>>("FiltedOrder");
	}
	catch (TinyROS::TinyROSException& e)
	{
		std::cout << e.what();
	}

	TinyROS::MessageCallback<TinyROS::SimpleObjectMessage<move>> callback(1);
	PriorityClass orderCallback(filtedOrderPub);
	callback.Register(&PriorityClass::CallbackInObject, orderCallback);
	try
	{
		moveOrderSub = TinyROS::NewSubscriber<TinyROS::SimpleObjectMessage<move>>("MoveOrder", callback);
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
#include <iostream>
#include "TinyROS/TinyROS.h"
#include <thread>

template<int length>
struct test
{
	int i;
	int j;
	int a[length];
};

void NormalCallback(TinyROS::SimpleObjectMessage<test<5>> msg)
{
	std::cout << "normal callback: I am normal function, I received: " << msg.Value.i << msg.Value.j << msg.Value.a[0] << msg.Value.a[1] << std::endl;
}

int main()
{

	//节点初始化
	try
	{
		TinyROS::Node::Init("Visual Induction");
	}
	catch (TinyROS::TinyROSException& e)
	{
		std::cout << e.what();
		return -1;
	}
	//定义节点
	TinyROS::Publisher* vision;
	TinyROS::Subscriber* pForSub;
	try
	{
		vision = TinyROS::NewPublisher<TinyROS::SimpleObjectMessage<test<5>>>("Visual Induction");
	}
	catch (TinyROS::TinyROSException& e)
	{
		std::cout << e.what();
		return -1;
	}

	TinyROS::MessageCallback<TinyROS::SimpleObjectMessage<test<5>>> callback(5);

	callback.Register(NormalCallback);

	try
	{
		pForSub = TinyROS::NewSubscriber<TinyROS::SimpleObjectMessage<test<5>>>("Visual Induction", callback);
	}
	catch (TinyROS::TinyROSException& e)
	{
		std::cout << e.what();
		return -1;
	}

	test<5> wu;
	wu.i = 0;
	wu.j = 1;
	wu.a[0] = 2;
	wu.a[1] = 3;
	TinyROS::SimpleObjectMessage<test<5>> msg(wu);

	while (true)
	{
		wu.i += 1;
		wu.j += 1;
		wu.a[0] += 1;
		wu.a[1] += 1;
		msg.Value = wu;
		vision->Publish(msg);

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(1s);
	}
	TinyROS::Node::Close();

}
#include <iostream>
#include "TinyROS/TinyROS.h"
#include "TinyROS/SharedMessageTypes.h"
#include <thread>

template<int length>
struct test
{
	int i;
	int j;
	int a[length];
};
void NormalCallback(RoboTax::MapMessage map)
{
	for (int i = 0; i < map.GetHeight(); i++)
	{
		for (int j = 0; j < map.GetWidth(); j++)
		{
			std::cout << (int)map.At(i, j) << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;
}
void NormalCallback2(RoboTax::CarVelocityMessage msg)
{
	std::cout <<"linear:"<< msg.Value.Linear <<"radius:" <<msg.Value.Radius<< std::endl;
}

class SampleClass
{
public:
    std::string Name;
    SampleClass(std::string name)
        : Name(name) { }
    // 类内部的方法作为回调函数，函数签名一样，但是稍后注册方法不同
    void CallbackInObject(TinyROS::SimpleObjectMessage<test<5>> msg)
    {
        std::cout << "object callback: I am a member function of " << this->Name << ", I received: " << msg.Value.i << msg.Value.j<< std::endl;
    }
    
    static void StaticMemberFuntion(TinyROS::SimpleObjectMessage<test<5>> msg)
    {
        std::cout << "static callback: I am a static member function of SampleClass, I received: " << msg.Value.i << msg.Value.j << std::endl;
    }
};

class SampleFunctionalObjectClass
{
public:
	std::string Name;
	SampleFunctionalObjectClass(std::string name)
		: Name(name) { }
	// 重载了括号，称为函数对象，可以像函数一样调用
	void operator()(TinyROS::SimpleObjectMessage<test<5>> msg)
	{
		std::cout << "functional object callback: I am an object named " << this->Name << ", I received: " << msg.Value.i << msg.Value.j << std::endl;
	}
};

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

	TinyROS::MessageCallback<RoboTax::CarVelocityMessage> callback(1);

	callback.Register(NormalCallback2);

	//SampleClass sampleObj("Noelle");
	//callback.Register(&SampleClass::CallbackInObject, sampleObj);

	//callback.Register(SampleClass::StaticMemberFuntion);

	//SampleFunctionalObjectClass sampleFunctionalObj("Barbara");
	//callback.Register(sampleFunctionalObj);
	try
	{
		pForSub = TinyROS::NewSubscriber<RoboTax::CarVelocityMessage>("FiltedOrder", callback);
	}
	catch (TinyROS::TinyROSException& e)
	{
		std::cout << e.what();
		return -1;
	}

	/*TinyROS::StringMessage msg("start");*/
	RoboTax::CarVelocity moveOrder;
	moveOrder.Linear = 0.2;
	moveOrder.Radius = 0.0;
	RoboTax::CarVelocityMessage msg(moveOrder);
	while (true)
	{
		pForPub->Publish(msg);
		std::cout << "send a rank2 order"<<std::endl;
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(3s);
	}
	TinyROS::Node::Close();

}
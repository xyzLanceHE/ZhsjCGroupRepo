#include <iostream>
#include "TinyROS/TinyROS.h"
#include <thread>

struct test
{
	int i;
	int j;
};

void NormalCallback(TinyROS::SimpleObjectMessage<test> msg)
{
	std::cout << "normal callback: I am normal function, I received: " << msg.value.i << msg.value.j<< std::endl;
}

class SampleClass
{
public:
    std::string Name;
    SampleClass(std::string name)
        : Name(name) { }
    // 类内部的方法作为回调函数，函数签名一样，但是稍后注册方法不同
    void CallbackInObject(TinyROS::SimpleObjectMessage<test> msg)
    {
        std::cout << "object callback: I am a member function of " << this->Name << ", I received: " << msg.value.i << msg.value.j<< std::endl;
    }
    
    static void StaticMemberFuntion(TinyROS::SimpleObjectMessage<test> msg)
    {
        std::cout << "static callback: I am a static member function of SampleClass, I received: " << msg.value.i << msg.value.j << std::endl;
    }
};

class SampleFunctionalObjectClass
{
public:
	std::string Name;
	SampleFunctionalObjectClass(std::string name)
		: Name(name) { }
	// 重载了括号，称为函数对象，可以像函数一样调用
	void operator()(TinyROS::SimpleObjectMessage<test> msg)
	{
		std::cout << "functional object callback: I am an object named " << this->Name << ", I received: " << msg.value.i << msg.value.j << std::endl;
	}
};

int main()
{

	//节点初始化
	try
	{
		TinyROS::Node::Init("Foolish Wu");
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
		pForPub = TinyROS::NewPublisher<TinyROS::SimpleObjectMessage<test>>("Foolish Wu");
	}
	catch (TinyROS::TinyROSException& e)
	{
		std::cout << e.what();
		return -1;
	}

	TinyROS::MessageCallback callback(5);

	callback.Register<TinyROS::SimpleObjectMessage<test>>(NormalCallback);

	SampleClass sampleObj("Noelle");
	callback.Register<TinyROS::SimpleObjectMessage<test>>(&SampleClass::CallbackInObject, sampleObj);

	callback.Register<TinyROS::SimpleObjectMessage<test>>(SampleClass::StaticMemberFuntion);

	SampleFunctionalObjectClass sampleFunctionalObj("Barbara");
	callback.Register<TinyROS::SimpleObjectMessage<test>>(sampleFunctionalObj);
	try
	{
		pForSub = TinyROS::NewSubscriber<TinyROS::SimpleObjectMessage<test>>("Foolish Wu", callback);
	}
	catch (TinyROS::TinyROSException& e)
	{
		std::cout << e.what();
		return -1;
	}

	test wu;
	wu.i = 0;
	wu.j = 1;
	TinyROS::SimpleObjectMessage<test> msg(wu);

	while (true)
	{
		wu.i += 1;
		wu.j += 1;
		msg.value = wu;
		pForPub->Publish(msg);

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(1s);
	}
	TinyROS::Node::Close();

}
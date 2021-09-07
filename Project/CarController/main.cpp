#include <iostream>
#include <string>
#include "TinyROS/TinyROS.h"
#include "CarController.h"

// 本节点的运行参数
std::string NodeName;   //节点名
std::string PortPath;	//串口路径
std::string TopicName;  //速度话题名

// 小车速度结构体
struct CarVelocity
{
	float Linear;
	float Radius;
};

// 后续可以考虑把该类型共享
using CarVelocityMessage = TinyROS::SimpleObjectMessage<CarVelocity>;

// 函数对象，将速度传递给一个CarControllerInterface
class VelocityForwarder
{
public:
	VelocityForwarder(RoboTax::CarControllerInterface* pController)
	{
		this->pController = pController;
	}
	void operator()(CarVelocityMessage velocityMsg)
	{
		this->pController->SetSpeed(velocityMsg.Value.Linear, velocityMsg.Value.Radius);
	}
private:
	RoboTax::CarControllerInterface* pController;
};


void ParseArgs(int argc, char* argv[])
{
	if (argc < 3)
	{
		std::cout << "参数数目不足，需要如下参数：节点名称、串口名称\n";
		throw std::invalid_argument("参数数目不足");
	}
	else
	{
		NodeName = argv[1];
		PortPath = argv[2];
		TopicName = "cmd/vel/";
		TopicName += NodeName;
	}
}


int main(int argc, char* argv[])
{

	ParseArgs(argc, argv);

	std::cout << "Initializing this as Node " << NodeName << std::endl;
	TinyROS::Node::Init(NodeName.c_str());

	RoboTax::CarControllerInterface controller(PortPath.c_str());
	VelocityForwarder vForwarder(&controller);

	TinyROS::MessageCallback<CarVelocityMessage> callback(1);
	TinyROS::Subscriber* velocitySubscriber;

	velocitySubscriber = TinyROS::NewSubscriber<CarVelocityMessage>(TopicName.c_str(), callback);
	callback.Register(vForwarder);

	std::cout << "Init success\n";

	while (true)
	{

	}

	TinyROS::Node::Close();


	return 0;
}


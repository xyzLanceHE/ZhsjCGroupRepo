#include <iostream>
#include <string>
#include "TinyROS/TinyROS.h"

int main(int argc, char* argv[])
{
	std::string nodeName;

	if (argc < 2)
	{
		std::cout << "该程序需要一个参数作为节点名称\n";
		return -1;
	}
	else
	{
		nodeName = argv[1];
	}
	std::cout << "Initializing this as Node " << nodeName << std::endl;
	TinyROS::Node::Init(nodeName.c_str());

	std::cout << "Init success\n";
	TinyROS::Node::Close();


	return 0;
}


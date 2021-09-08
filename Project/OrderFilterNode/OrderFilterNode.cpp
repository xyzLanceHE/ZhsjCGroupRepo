#include <iostream>
#include "TinyROS/TinyROS.h"
#include <thread>

int main(){
	try
	{
		TinyROS::Node::Init("OrderFilterNode");
	}
	catch (TinyROS::TinyROSException& e)
	{
		std::cout << e.what();
		return -1;
	}

    TinyROS::Publisher* publishStart;
    try
    {
        publishStart = TinyROS::NewPublisher<TinyROS::StringMessage>("start");
    }
    catch (TinyROS::TinyROSException& e)
    {
        std::cout << e.what();
        return -1;
    }


    while (true)
    {
        std::string startmsg("Start Now");
        TinyROS::StringMessage msg(startmsg);
        publishStart->Publish(msg);


        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
    }

	TinyROS::Node::Close();
}
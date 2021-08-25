#include <iostream>
#include "TinyROS/TinyROS.h"

int main()
{
    TinyROS::Node::HelloWorld();
    try {
        TinyROS::Node::Init();
    }
    catch (TinyROS::NodeInitializeFailedException& e)
    {
        std::cout << e.what();
    }
    int hang;
    std::cin >> hang;
    return 0;
}
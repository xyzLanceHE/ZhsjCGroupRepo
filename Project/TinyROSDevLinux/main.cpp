﻿#include <iostream>
#include <string>
#include "TinyROS/TinyROS.h"

int main()
{
    TinyROS::Publisher<TinyROS::SimpleObjectMessage<int>> p("");
    try
    {
        TinyROS::Node::Init("hello");
    }
    catch(TinyROS::TinyROSException& e)
    {
        std::cout << e.what();
    }
    int hang;
    std::cin >> hang;
    return 0;
}
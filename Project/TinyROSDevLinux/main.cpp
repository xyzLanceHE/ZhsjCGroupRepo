#include <iostream>
#include "TinyROS/TinyROS.h"

int main()
{
    TinyROS::Node::Init("hello");
    int hang;
    std::cin >> hang;
    return 0;
}
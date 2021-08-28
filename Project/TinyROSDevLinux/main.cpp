#include <iostream>
#include <string>
#include "TinyROS/TinyROS.h"

using namespace TinyROS;
void TestCallback(SimpleObjectMessage<int> msg)
{
    std::cout << "normal callback:" << msg.value << std::endl;
}

class TestClass
{
public:
    void TestObjectCallback(SimpleObjectMessage<int> msg)
    {
        std::cout << "object callback:" << msg.value << std::endl;
    }
};

int main()
{


    Subscriber<SimpleObjectMessage<int>> s1("a", TestCallback);
    TestClass obj1;
    Subscriber< SimpleObjectMessage<int>> s2("b", &obj1, &TestClass::TestObjectCallback);

    while (true)
    {
        int num;
        std::cin >> num;
        s1.InvokeTest(num);
        s2.InvokeTest(num);
    }
 

    int hang;
    std::cin >> hang;
    return 0;
}
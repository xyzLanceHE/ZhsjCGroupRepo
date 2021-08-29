#include <iostream>
// 需要进行的准备：（本Demo已经设置好了，可以参考）
// 需要将C++标准改为14或者更高
// 需要在C/C++的命令行中添加参数 -pthread
// 将Share项目添加为本项目的引用，即可直接访问到Share下的资源
// 预处理器中预定义宏 TINYROS_ON_LINUX，否则会报错
// 项目中添加资源文件 libcrypto_x64.a （Share/OpenSSL/lib下），否则会链接错误
// 此Demo针对x64，如果是在树莓派上，需要将项目的目标平台改成ARM，把上述静态库换成libcrypto_arm.a
#include "TinyROS/TinyROS.h"
#include <thread>

// 回调函数的形式：返回值是void，参数是收到的消息
void NormalCallback(TinyROS::StringMessage msg)
{
    std::cout << "normal callback: I received: " << msg.GetValue() << std::endl;
}

class SampleClass
{
public:
    std::string Name;
    SampleClass(std::string name)
        : Name(name) { }
    // 类内部的方法作为回调函数，函数签名一样，但是稍后注册方法不同
    void CallbackInObject(TinyROS::StringMessage msg)
    {
        std::cout << "object callback: I am " << this->Name << ", I received: " << msg.GetValue() << std::endl;
    }
};

int main()
{
    std::cout << (typeid(TinyROS::StringMessage)).name() << std::endl << (typeid(TinyROS::StringMessage)).hash_code() << std::endl;
    // TinyROS的所有内容都位于TinyROS命名空间下，（不建议using命名空间，避免冲突）
    // 关于Node：此类用于将本进程初始化为TinyROS的节点，请在程序开始时使用
    try
    {
        // 一句话即可。参数是本节点的名字，在局域网中必须是唯一的
        TinyROS::Node::Init("Hello world listen");
    }
    catch (TinyROS::TinyROSException& e)
    {
        std::cout << e.what();
        return -1;
    }

    // Node初始化完成之后，可以定义Publisher和Subscriber
    // 本Demo演示Subscriber
    // Subscriber是一个模板类，模板参数为1至2个，第一个参数是消息的类型
    TinyROS::Subscriber<TinyROS::StringMessage>* helloReceiver1;
    // 第二个参数可选，如果回调函数是类的方法，则填类名
    TinyROS::Subscriber<TinyROS::StringMessage, SampleClass>* helloReceiver2;
    // 既然类的方法做回调函数，肯定有一个类的对象
    SampleClass sampleObj("Foo");
    try
    {
        // 构造函数的第一个参数是话题名称。话题不存在时，会自动创建
        // 话题一旦创建，消息的类型是确定的，后续新的Subscriber/Publisher订阅或者发布到此话题，需要与之匹配，否则会抛异常
        // 普通回调函数， 第二个参数直接填函数名
        helloReceiver1 = new TinyROS::Subscriber<TinyROS::StringMessage>("HelloWorldTopic", NormalCallback);
        // 类的方法做回调函数，按照如下格式填写: 第二个参数是对象的指针，第三个参数固定写法为 &类名::方法名
        helloReceiver2 = new TinyROS::Subscriber<TinyROS::StringMessage, SampleClass>("HelloWorldTopic", &sampleObj, 
            &SampleClass::CallbackInObject);
        // ps. 类的静态方法做回调函数，和普通回调函数的写法一致
    }
    catch (TinyROS::TinyROSException& e)
    {
        std::cout << e.what();
        return -1;
    }

    
    // Subscriber一旦创建便持续生效，但是并不会阻塞，需要自己保持程序运行
    while (true)
    {

    }


    // 程序退出之前，关闭此Node
    TinyROS::Node::Close();
}


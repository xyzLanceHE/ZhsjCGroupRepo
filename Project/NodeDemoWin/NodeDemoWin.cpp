#include <iostream>
// 需要进行的准备：（本Demo已经设置好了，可以参考）
// 将Share项目添加为本项目的引用，即可直接访问到Share下的资源
// 预处理器中预定义宏 TINYROS_ON_WINDOWS，否则会报错
// 项目中添加资源文件 libcrypto_static_x64.lib （Share/OpenSSL/lib下），否则会链接错误
#include "TinyROS/TinyROS.h"
#include <thread>

int main()
{

    // TinyROS的所有内容都位于TinyROS命名空间下，（不建议using命名空间，避免冲突）
    // 关于Node：此类用于将本进程初始化为TinyROS的节点，请在程序开始时使用
    try
    {
        // 一句话即可。参数是本节点的名字，在局域网中必须是唯一的
        TinyROS::Node::Init("Hello World Talker");
    }
    catch (TinyROS::TinyROSException& e)
    {
        std::cout << e.what();
        return -1;
    }

    // Node初始化完成之后，可以定义Publisher和Subscriber
    // 本Demo演示Publisher
    // Publisher是一个模板类，模板参数是消息的类型
    TinyROS::Publisher<TinyROS::StringMessage>* helloer;
    try
    {
        // 构造函数的参数是话题名称。话题不存在时，会自动创建
        // 话题一旦创建，消息的类型是确定的，后续新的Subscriber/Publisher订阅或者发布到此话题，需要与之匹配，否则会抛异常
        helloer = new TinyROS::Publisher<TinyROS::StringMessage>("HelloWorldTopic");
    }
    catch (TinyROS::TinyROSException& e)
    {
        std::cout << e.what();
        return -1;
    }

    // Publisher 成功创建之后，就可以调用Publish方法
    int num = 0;
    std::string helloStr("Hello wrold");
    while (true)
    {
        std::string msgStr = helloStr + std::to_string(num);
        TinyROS::StringMessage msg(msgStr);
        num++;
        helloer->Publish(msg);
        
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1s);
    }
    


    // 程序退出之前，关闭此Node
    TinyROS::Node::Close();
}


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
    std::cout << "normal callback: I am normal function, I received: " << msg.GetValue() << std::endl;
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
        std::cout << "object callback: I am a member function of " << this->Name << ", I received: " << msg.GetValue() << std::endl;
    }
    
    static void StaticMemberFuntion(TinyROS::StringMessage msg)
    {
        std::cout << "static callback: I am a static member function of SampleClass, I received: " << msg.GetValue() << std::endl;
    }
};

class SampleFunctionalObjectClass
{
public:
    std::string Name;
    SampleFunctionalObjectClass(std::string name)
        : Name(name) { }
    // 重载了括号，称为函数对象，可以像函数一样调用
    void operator()(TinyROS::StringMessage msg)
    {
        std::cout << "functional object callback: I am an object named " << this->Name << ", I received: " << msg.GetValue() << std::endl;
    }
};

int main()
{

    // TinyROS的所有内容都位于TinyROS命名空间下，（不建议using命名空间，避免冲突）
    // 关于Node：此类用于将本进程初始化为TinyROS的节点，请在程序开始时使用
    try
    {
        // 一句话即可。参数是本节点的名字，在局域网中必须是唯一的
        TinyROS::Node::Init("Hello world listener");
    }
    catch (TinyROS::TinyROSException& e)
    {
        std::cout << e.what();
        return -1;
    }

    // Node初始化完成之后，可以定义Publisher和Subscriber
    // 本Demo演示Subscriber
    // Subscriber的构造函数被禁止调用，需要声明指针
    TinyROS::Subscriber* helloReceiver;
    
    // 在新建一个Subscriber之前，需要新建一个MessageCallback类的对象
    // 请确保此对象的生命期不小于Subscriber
    // MessageCallback的构造函数有一个参数，是最大能注册的回调函数个数，默认为3，根据需要填写即可
    TinyROS::MessageCallback callback(5);

    // callback对象具有注册、取消注册两种方法，可以在任何时候使用，不论是在新建Subscriber之前还是之后
    // 但是，不论是注册还是取消注册，不论注册哪种函数，都需要一个模板参数，填的是消息的类型
    // 请确保同一个callback注册的所有消息类型都相同，并且与Subscriber也是同一个，否则会出错

    // 注册一个普通函数：
    callback.Register<TinyROS::StringMessage>(NormalCallback);

    // 注册一个类的静态成员函数，与注册普通函数类似
    callback.Register<TinyROS::StringMessage>(SampleClass::StaticMemberFuntion);
    
    // 另外两种函数，放到新建Subscriber之后演示

    try
    {
        // 使用NewSubscriber函数申请一个新的Subscriber并得到指向它的指针
        // 该函数有一个模板参数，是消息的类型
        // 该函数有两个参数，第一个是话题名称。话题不存在时，会自动创建
        // 第二个参数是回调函数对象，请再次注意消息类型的匹配
        helloReceiver = TinyROS::NewSubscriber<TinyROS::StringMessage>("hello", callback);


        // 话题一旦创建，消息的类型是确定的，后续新的Subscriber/Publisher订阅或者发布到此话题，需要与之匹配，否则会抛异常
    }
    catch (TinyROS::TinyROSException& e)
    {
        std::cout << e.what();
        return -1;
    }

    // 注册一个类的对象的成员函数：
    // 对象的成员方法做回调函数，首先得有一个类的对象
    SampleClass sampleObj("Noelle");
    // 注意写法 ，第一个参数写法是固定的，为 &类名::函数名 ，第二个参数是要回调的对象
    callback.Register<TinyROS::StringMessage>(&SampleClass::CallbackInObject, sampleObj);
    // 请确保此对象的生命期不小于callback，或者在对象被销毁之前，及时调用Unregister方法
    
    // 注册一个函数对象：
    // 首先新建一个函数对象：
    SampleFunctionalObjectClass sampleFunctionalObj("Barbara");
    // 写法与普通函数类似
    callback.Register<TinyROS::StringMessage>(sampleFunctionalObj);
    // 同样，请确保此对象的生命期不小于callback
    
    // Subscriber一旦创建便持续生效，但是并不会阻塞，需要自己保持程序运行，否则程序就退出了
    while (true)
    {

    }


    // 程序退出之前，关闭此Node
    TinyROS::Node::Close();
}


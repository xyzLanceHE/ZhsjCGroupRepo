#include <iostream>
// 需要进行的准备：（本Demo已经设置好了，可以参考）
// 将Share项目添加为本项目的引用，即可直接访问到Share下的资源
// 预处理器中预定义宏 TINYROS_ON_WINDOWS，否则会报错
// 项目中添加资源文件 libcrypto_static_x64.lib （Share/OpenSSL/lib下），否则会链接错误
#include "TinyROS/TinyROS.h"


int main()
{
    // TinyROS的所有内容都位于TinyROS命名空间下，（不建议using命名空间，避免冲突）
    // 关于Master：此类只有几个静态方法，按照以下顺序依次调用即可
    // 一个局域网只有一个Master，并且Master跑起来之后也不用管他，本程序的内容基本上是固定的

    // step1：加载设置
    // 请确保运行路径正确，并且配置内容有效，尤其是本机IP（ListenIP），每次换网都需要手动设置
    TinyROS::Master::LoadConfig("MasterConfig.json");
    try
    {
        // step2：启动
        TinyROS::Master::Run();
    }
    catch (TinyROS::TinyROSException& e)
    {
        // 启动出现问题会显示在这里
        std::cout << e.what();
        return 0;
    }
    // step3：（可选）保持挂起。从这里开始也可以进行自己的操作。
    TinyROS::Master::Spin();
    // step4：（可选）记录一下上次运行的配置。
    TinyROS::Master::SaveConfig("MasterLastRun.json");

    return 0;
}



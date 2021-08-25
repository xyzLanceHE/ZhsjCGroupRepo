#include <iostream>
#include "TinyROS/TinyROS.h"

int main()
{

    try
    {
        TinyROS::Master::LoadConfig("MasterConfig.json");
        TinyROS::Master::Run();
    }
    catch (TinyROS::TinyROSException& e)
    {
        std::cout << e.what() << std::endl;
    }
    catch (std::exception& e2)
    {
        std::cout << e2.what() << std::endl;
    }
    TinyROS::Master::SaveConfig("MasterLastRun.json");
    int hang;
    std::cin >> hang;
    return 0;
}
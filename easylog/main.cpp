#include "../Common/easylog.h"
#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <Windows.h>

void foo()
{
    // do stuff...
    LOG_INFO("start");
    for (size_t i = 0; i < 100; i++)
    {
        LOG_INFO("-------- %d", i);
        Sleep(1);
    }
    LOG_INFO("end");
}

void bar(int x)
{
    // do stuff...
    LOG_INFO("start");
    for (size_t i = 0; i < 100; i++)
    {
        LOG_INFO("~~~~~~~~~~ %d", i);
        Sleep(1);
    }
    LOG_INFO("end");
}

int main()
{
    EasyLog::init(L"D:\\eee.txt");
    LOG_INFO("START");

    std::thread first(foo);     // spawn new thread that calls foo()
    std::thread second(bar, 0);  // spawn new thread that calls bar(0)

    for (size_t i = 0; i < 100; i++)
    {
        LOG_INFO("******** %d", i);
        Sleep(1);
    }

    // synchronize threads:
    first.join();                // pauses until first finishes
    second.join();               // pauses until second finishes

    LOG_INFO("END");
    return 0;
}
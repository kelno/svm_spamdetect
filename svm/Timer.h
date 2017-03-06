#ifndef H_TIMER
#define H_TIMER

#include <string>
#include "time.h"
#include <iostream>

class Timer
{
public:
    Timer(std::string txt)
        : txt(txt)
    {
        start_time = time(nullptr);
    }

    //return total time
    time_t stop(bool print = true)
    {
        time_t stop_time = time(nullptr);
        time_t total_time = stop_time - start_time;
        if(print)
            std::cout << txt << total_time << std::endl;
        return total_time;
    }
private:
    time_t start_time;
    std::string txt;
};

#endif H_TIMER
#include "game_loop.h"

#include <chrono>
#include <thread>
#include <iostream>

using namespace std::chrono_literals;

// we use a fixed timestep of 1 / (60 fps) = 16 milliseconds
constexpr std::chrono::nanoseconds timestep(16666666ns);

void GameLoop::run(std::function<void(float)> update) {
    
    using clock = std::chrono::high_resolution_clock;

    std::chrono::nanoseconds total_frame_time = timestep;

    std::chrono::nanoseconds sleepy_time = timestep;

    int num = 0;
    std::chrono::nanoseconds sum = 0ns;
    
    while(running) {
        
        auto start = clock::now();
        
        update(total_frame_time.count() / 1000000.f);
        
        auto update_duration = std::chrono::duration_cast<std::chrono::nanoseconds> (clock::now() - start);

        sleepy_time = timestep - update_duration - (total_frame_time - sleepy_time - update_duration);
        std::this_thread::sleep_for(sleepy_time);

        total_frame_time = std::chrono::duration_cast<std::chrono::nanoseconds> (clock::now() - start);

        num++;
        sum += total_frame_time;
        if (num >= 120) {
            std::cout << 1000000000.f / (sum.count()/num) << " fps" << std::endl;
            num = 0;
            sum = 0ns;
        }

    }
    
}

void GameLoop::setQuitting() {
    running = false;
}


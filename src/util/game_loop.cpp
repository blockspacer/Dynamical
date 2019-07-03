#include "game_loop.h"

#include <chrono>
#include <thread>
#include <iostream>

using namespace std::chrono_literals;

// we use a fixed timestep of 1 / (60 fps) = 16 milliseconds
constexpr std::chrono::nanoseconds timestep(16666666ns);

void GameLoop::run() {
    
    using clock = std::chrono::high_resolution_clock;

    auto last_frame = clock::now() - timestep;
    
    while(running) {
        
        auto before_update = clock::now();
        
        auto time_since_last_frame = std::chrono::duration_cast<std::chrono::nanoseconds> (before_update - last_frame);
        last_frame = before_update;
        
        update(std::chrono::duration_cast<std::chrono::microseconds> (time_since_last_frame).count() / 1000.f);
        
        auto after_update = clock::now();
        
        auto update_duration = std::chrono::duration_cast<std::chrono::nanoseconds> (after_update - before_update);
        
        std::chrono::nanoseconds sleepy_time(timestep - update_duration);

        std::this_thread::sleep_for(sleepy_time);
        
        std::cout << 1000000000./time_since_last_frame.count() << std::endl;

    }
    
    quit();
    
}

void GameLoop::setQuitting() {
    running = false;
}


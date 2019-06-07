#include "game_loop.h"

#include <chrono>
#include <thread>
#include <iostream>

using namespace std::chrono_literals;

// we use a fixed timestep of 1 / (60 fps) = 16 milliseconds
constexpr std::chrono::nanoseconds timestep(16666666ns);

void GameLoop::run() {
    
    using clock = std::chrono::high_resolution_clock;
    
    auto frame_time = timestep;
    
    auto last_frame = clock::now();
    
    auto full_frametime = timestep;

    while(running) {
        
        auto last_time = clock::now();
        
        update(std::chrono::duration_cast<std::chrono::microseconds> (full_frametime).count() / 1000.f);
        
        auto current_time = clock::now();
        full_frametime = std::chrono::duration_cast<std::chrono::nanoseconds> (current_time - last_frame);
        last_frame = current_time;
        
        frame_time = std::chrono::duration_cast<std::chrono::nanoseconds> (current_time - last_time);
        
        std::chrono::nanoseconds sleepy_time(timestep - frame_time);

        std::this_thread::sleep_for(sleepy_time);
        
        //std::cout << 1000000000./full_frametime.count() << std::endl;

    }
    
    quit();
    
}

void GameLoop::setQuitting() {
    running = false;
}


#include "system_list.h"

#include "logic/components/inputc.h"

#include <SDL.h>

#include <iostream>

#include <unordered_map>

std::unordered_map<SDL_Scancode, Action> actionMap = {
    {SDL_SCANCODE_W, Action::FORWARD},
    {SDL_SCANCODE_S, Action::BACKWARD},
    {SDL_SCANCODE_A, Action::LEFT},
    {SDL_SCANCODE_D, Action::RIGHT},
    {SDL_SCANCODE_LCTRL, Action::SPRINT},
    {SDL_SCANCODE_SPACE, Action::UP},
    {SDL_SCANCODE_LSHIFT, Action::DOWN},
    {SDL_SCANCODE_M, Action::MENU},
    {SDL_SCANCODE_K, Action::DEBUG}
};

void InputSys::preinit() {
    
    InputC& input = reg.set<InputC>();
    input.mouseFree = false;
    
}

void InputSys::init() {
    
    SDL_Window* win = reg.ctx<SDL_Window*>();
    
    int w, h;
    SDL_GetWindowSize(win, &w, &h);
    SDL_WarpMouseInWindow(win, w / 2, h / 2);
    
}

void InputSys::tick() {
    
    InputC& input = reg.ctx<InputC>();
    
    input.mouseRight = input.mouseLeft = input.mouseMiddle = false;
    
    SDL_Window* win = reg.ctx<SDL_Window*>();

    auto flags = SDL_GetWindowFlags(win);

    input.focused = flags & SDL_WINDOW_MOUSE_FOCUS;
    
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        
        if(e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED) {
            
            input.on.set(Action::RESIZE);
            
        } else if(e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)) {
            
            input.on.set(Action::EXIT);
            
        } else if(e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
            
            try {
                input.on.set(actionMap.at(e.key.keysym.scancode), e.type == SDL_KEYDOWN);
            } catch(std::out_of_range& err) {}
            
        } else if(e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) {
            
            switch(e.button.button) {
                case SDL_BUTTON_LEFT:
                    input.on.set(Action::PRIMARY, e.type == SDL_MOUSEBUTTONDOWN);
                    if(e.type == SDL_MOUSEBUTTONDOWN) input.mouseLeft = true;
                    break;
                case SDL_BUTTON_RIGHT:
                    input.on.set(Action::SECONDARY, e.type == SDL_MOUSEBUTTONDOWN);
                    if(e.type == SDL_MOUSEBUTTONDOWN) input.mouseRight = true;
                    break;
                case SDL_BUTTON_MIDDLE:
                    input.on.set(Action::TERTIARY, e.type == SDL_MOUSEBUTTONDOWN);
                    if(e.type == SDL_MOUSEBUTTONDOWN) input.mouseMiddle = true;
                    break;
            }
            
        } if(e.type == SDL_MOUSEWHEEL) {
            
            if (e.wheel.x > 0) input.mouseWheel.x += 1;
            else if (e.wheel.x < 0) input.mouseWheel.x -= 1;
            if (e.wheel.y > 0) input.mouseWheel.y += 1;
            else if (e.wheel.y < 0) input.mouseWheel.y -= 1;
            
        }
        
    }
    
    if (input.focused) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        
        int w, h;
        SDL_GetWindowSize(win, &w, &h);
        if(!input.mouseFree) SDL_WarpMouseInWindow(win, w / 2, h / 2);
        
        input.mousePos = glm::ivec2(x, y);
        input.mouseDiff = glm::ivec2(x - w / 2, y - h / 2);
    }
    
}

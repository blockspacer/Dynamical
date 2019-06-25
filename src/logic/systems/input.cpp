#include "system_list.h"

#include "logic/components/input.h"

#include <SDL.h>

#include <iostream>

#include <unordered_map>

std::unordered_map<SDL_Scancode, Action> actionMap = {
    {SDL_SCANCODE_Z, Action::FORWARD},
    {SDL_SCANCODE_S, Action::BACKWARD},
    {SDL_SCANCODE_Q, Action::LEFT},
    {SDL_SCANCODE_D, Action::RIGHT}
};

void InputSys::init(entt::registry& reg) {
    
    reg.set<Input>();
    
}

void InputSys::tick(entt::registry& reg) {
    
    Input& input = reg.ctx<Input>();
    
    //SDL_Window* win = reg.ctx<SDL_Window*>();
    
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        
        if(e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_RESIZED) {
            
            input.on.set(Action::RESIZE);
            
        } else if(e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.scancode == SDL_SCANCODE_ESCAPE)) {
            
            input.on.set(Action::EXIT);
            
        } else if(e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
            
            try {
                input.on.set(actionMap.at(e.key.keysym.scancode), e.type == SDL_KEYDOWN);
            } catch(std::out_of_range e) {}
            
        } else if(e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) {

            switch(e.button.button) {
                case SDL_BUTTON_LEFT:
                    input.on.set(Action::PRIMARY, e.type == SDL_MOUSEBUTTONDOWN);
                    break;
                case SDL_BUTTON_RIGHT:
                    input.on.set(Action::SECONDARY, e.type == SDL_MOUSEBUTTONDOWN);
                    break;
            }
            
        }
        
    }
    
    int x, y;
    SDL_GetMouseState(&x, &y);
    input.mousePos = glm::ivec2(x,y);
    
}

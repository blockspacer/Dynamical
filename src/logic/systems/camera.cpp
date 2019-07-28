#include "system_list.h"

#define _USE_MATH_DEFINES
#include <cmath>

#include "logic/components/inputc.h"

#include "logic/components/camerac.h"

void CameraSys::init() {
    
    reg.set<CameraC>();
    
}

void CameraSys::tick() {
    
    CameraC& camera = reg.ctx<CameraC>();
    
    const InputC& input = reg.ctx<InputC>();
    
    constexpr float speed = 5.f;
    
    if(input.on[Action::FORWARD]) {
        camera.pos.x -= speed * std::sin(camera.yAxis);//*dt;
        camera.pos.z -= speed * std::cos(camera.yAxis);//*dt;
    } if(input.on[Action::BACKWARD]) {
        camera.pos.x += speed * std::sin(camera.yAxis);//*dt;
        camera.pos.z += speed * std::cos(camera.yAxis);//*dt;
    } if(input.on[Action::LEFT]) {
        camera.pos.x -= speed * std::sin(M_PI/2.0f + camera.yAxis);//*dt;
        camera.pos.z -= speed * std::cos(M_PI/2.0 + camera.yAxis);//*dt;
    } if(input.on[Action::RIGHT]) {
        camera.pos.x += speed * std::sin(M_PI/2.0 + camera.yAxis);//*dt;
        camera.pos.z += speed * std::cos(M_PI/2.0 + camera.yAxis);//*dt;
    } if(input.on[Action::UP]) {
        camera.pos.y += speed;
    } if(input.on[Action::DOWN]) {
        camera.pos.y -= speed;
    }
    
    camera.yAxis -= (input.mouseDiff.x) * (M_PI*0.1/180.);
    camera.xAxis = std::max(std::min(camera.xAxis - (input.mouseDiff.y) * (M_PI*0.1/180.), M_PI/2.), -M_PI/2.);
    
}

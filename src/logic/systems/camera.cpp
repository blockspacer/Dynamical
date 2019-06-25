#include "system_list.h"

#include "logic/components/input.h"

#include "logic/components/camera.h"

void CameraSys::init(entt::registry& reg) {
    
    reg.set<Camera>();
    
}

void CameraSys::tick(entt::registry& reg) {
    
    Camera& camera = reg.ctx<Camera>();
    
    const Input& input = reg.ctx<Input>();
    
    if(input.on[Action::FORWARD]) {
        camera.pos.z += 1;
    } else if(input.on[Action::BACKWARD]) {
        camera.pos.z -= 1;
    } else if(input.on[Action::LEFT]) {
        camera.pos.x += 1;
    }  else if(input.on[Action::RIGHT]) {
        camera.pos.x -= 1;
    }
    
}

#include "camera.h"

#include "device.h"
#include "swapchain.h"

#include "logic/components/camerac.h"

#include "glm/gtc/matrix_transform.hpp"

Camera::Camera(int width, int height) {
    setup(width, height);
}

void Camera::setup(int width, int height) {
    
    projection = glm::perspective(render_fov, (float) width/height, render_min, render_distance);
    
}

void Camera::update(entt::registry& reg) {
    
    CameraC& cam = reg.ctx<CameraC>();
    
    view = glm::mat4(1.0);
    view = glm::translate(view, cam.pos);
    view = glm::rotate(view, (float) cam.yAxis, glm::vec3(0.0f, 1.0f, 0.0f));
    view = glm::rotate(view, (float) cam.xAxis, glm::vec3(1.0f, 0.0f, 0.0f));
    view = glm::inverse(view);
    
    viewProjection = projection * view;
    
}

glm::mat4 & Camera::getProjection() {
    return projection;
}

glm::mat4 & Camera::getView() {
    return view;
}

glm::mat4 & Camera::getViewProjection() {
    return viewProjection;
}


Camera::~Camera() {
    
}

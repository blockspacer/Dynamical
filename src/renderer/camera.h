#ifndef CAMERA_H
#define CAMERA_H

#include "entt/entt.hpp"
#include "glm/glm.hpp"

constexpr float render_distance = 1000.f;
constexpr float render_min = 0.1f;
constexpr float render_fov = 70.f;

class Camera {
public:
    Camera(int width, int height);
    void setup(int width, int height);
    void update(entt::registry& reg);
    ~Camera();
    
    glm::mat4& getProjection();
    glm::mat4& getView();
    glm::mat4& getViewProjection();
    
    glm::vec3 getViewPosition();
    
private:
    
    glm::vec3 position;
    
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 viewProjection;
    
};

#endif

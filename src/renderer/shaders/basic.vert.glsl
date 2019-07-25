#version 450

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_uv;

layout(location = 0) out vec3 v_position;
layout(location = 1) out vec3 v_normal;
layout(location = 2) out vec2 v_uv;

out gl_PerVertex {
    vec4 gl_Position;
};

layout(binding = 0) uniform UBO {
    
    mat4 vp;
    
};

void main() {

    gl_Position = vp * vec4(a_pos, 1.0);
    v_position = a_pos;
    v_normal = a_normal;
    v_uv = a_uv;
    
}

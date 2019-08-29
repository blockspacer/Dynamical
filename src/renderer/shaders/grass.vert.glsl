#version 450

layout(location = 0) in vec3 a_pos;
layout(location = 1) in vec3 a_normal;
layout(location = 2) in vec2 a_uv;

layout(location = 0) out vec3 v_position;
layout(location = 1) out vec3 v_normal;
layout(location = 2) out vec2 v_uv;

layout(push_constant ) uniform Args {
    vec4 base_normal;
    float tile_size;
    float grass_height;
};

out gl_PerVertex {
    vec4 gl_Position;
};

layout(std140, set = 0, binding = 0) uniform UBO {
    mat4 viewproj;
    vec4 viewpos;
};

void main() {

    vec3 position = a_pos - vec3(0, -1, 0) * grass_height * normalize(vec3(0.5, 10, 1.5)).y;
    gl_Position = viewproj * vec4(position, 1.0);
    v_position = position;
    v_normal = a_normal;
    v_uv = a_uv;
    
}

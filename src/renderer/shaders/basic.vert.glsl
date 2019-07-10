#version 450

layout(location = 0) in vec4 a_pos;

layout(location = 0) out vec3 v_position;

out gl_PerVertex {
    vec4 gl_Position;
};

layout(binding = 0) uniform UBO {
    
    mat4 vp;
    
};

void main() {

    gl_Position = vp * vec4(a_pos);
    v_position = a_pos.xyz;
    gl_Position.y = -gl_Position.y;
    
}

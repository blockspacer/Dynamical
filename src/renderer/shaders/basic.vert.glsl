#version 450

layout(location = 0) in vec3 a_pos;

out gl_PerVertex {
    vec4 gl_Position;
};

void main() {
    vec4 position = vec4(a_pos, 1);
}

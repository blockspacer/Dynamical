#version 450

layout(location = 0) in vec3 v_position;

layout(location = 0) out vec4 outColor;

void main() {

    outColor = vec4(min(abs(v_position/50.), 1), 1);
    
    vec3 normal = normalize(cross(dFdx(v_position), dFdy(v_position)));
    float light = abs(dot(normal, vec3(0.5,-1.,0.5)));
    outColor.rgb *= light;
    
}

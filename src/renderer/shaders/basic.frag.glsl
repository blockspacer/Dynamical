#version 450

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_uv;

layout(location = 0) out vec4 outColor;

const int chunk_size = 8*8*5;

void main() {
    
    float separator = min(length(1.0 - min(abs((ivec3(v_position.xyz)%chunk_size)/2.), 1.0)) * 1000., 1.0);
    vec3 spos = v_position/length(v_position);
    outColor = vec4(abs(spos.x), separator, abs(spos.z), 1);
    
    //vec3 normal = normalize(cross(dFdx(v_position), dFdy(v_position)));
    vec3 normal = normalize(v_normal);
    float light = max(dot(normal, vec3(0.5,-1.,0.5)), 0);
    outColor.rgb = outColor.rgb * (light*0.7 + 0.3);
    
}

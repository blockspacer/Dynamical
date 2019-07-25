#version 450

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_uv;

layout(location = 0) out vec4 outColor;

void main() {
    
    vec3 spos = v_position/length(v_position);
    outColor = vec4(abs(spos.x), 1-abs(spos.x), abs(spos.z), 1);
    
    //vec3 normal = (cross(dFdx(v_position), dFdy(v_position)));
    vec3 normal = normalize(v_normal);
    float light = max(dot(normal, vec3(0.5,-1.,0.5)), 0);
    outColor.rgb = outColor.rgb * (light*0.7 + 0.3);
    
}

#version 450

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_uv;

layout(location = 0) out vec4 outColor;

layout(constant_id = 0) const int num_angles = 0;
layout(constant_id = 1) const int num_samples = 0;

layout(set = 1, binding = 0) uniform sampler3D u_raycast;

layout(constant_id = 2) const int chunk_size = 8*8*8;

layout(std140, set = 0, binding = 0) uniform UBO {
    mat4 viewproj;
    vec4 viewpos;
};

void main() {
    
    /*
    float separator = min(length(1.0 - min(abs((ivec3(v_position.xyz)%chunk_size)/2.), 1.0)) * 1000., 1.0);
    vec3 spos = v_position/length(v_position);
    outColor = vec4(abs(spos.x), separator, abs(spos.z), 1);
    */
    
    //vec3 normal = normalize(cross(dFdx(v_position), dFdy(v_position)));
    
    vec3 normal = normalize(v_normal);
    
    if(-normal.y * normal.y * normal.y > 0.5) {
    
        vec3 dir = normalize(v_position.xyz - viewpos.xyz);
        
        float angle = atan(dir.z, dir.x);
        
        vec4 ray = texture(u_raycast, vec3(v_position.xz/10., angle/2/3.141));
        
        float depth = (1/ray.r - 1) * -dir.y / length(dir.xz);
        
        if(depth < 5.) {
            
            normal = ray.gba;
            
            outColor.rgb = vec3(0, 1., 0);
            
            float light = max(dot(normal, vec3(0.5,-1.,0.5)), 0.4);
            outColor.rgb = outColor.rgb * (light*0.7 + 0.3);
            
            return;
        }
        
    }
    
    discard;
    
}

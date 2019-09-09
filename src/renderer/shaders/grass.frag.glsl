#version 450

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_uv;

layout(location = 0) out vec4 outColor;

layout(push_constant ) uniform Args {
    vec4 base_normal;
    float tile_size;
    float grass_height;
    float noise_frequency;
    float noise_amplitude;
};

layout(set = 1, binding = 0) uniform sampler3D u_raycast;

layout(set = 1, binding = 1) uniform sampler2D u_noise;

layout(std140, set = 0, binding = 0) uniform UBO {
    mat4 viewproj;
    vec4 viewpos;
};

const int num_layers = 3;
const vec3 normals[num_layers] = {
    vec3(0, 1, 0),
    vec3(0, 1, 0),
    vec3(0, 1, 0)
};

void main() {
    
    //vec3 normal = normalize(cross(dFdx(v_position), dFdy(v_position)));
    
    float final_depth = 100.;
    
    vec3 normal = normalize(v_normal);
    
    for(int i = 0; i<num_layers; i++) {
        
        vec3 new_normal = base_normal.xyz + normals[i] + 0.1*vec3(0.5*sin(v_position.x/10. + v_position.z/10. + viewpos.w/4. * 2.*3.141), 0, cos(v_position.x/10. + v_position.z/10. + viewpos.w/4. * 2.*3.141));
        new_normal = normalize(new_normal);
        
        mat3 TBN = (mat3(
            1./tile_size, 0, new_normal.x,
            0,            0, new_normal.y,
            0, 1./tile_size, new_normal.z
        ));
        
        mat3 invTBN = inverse(TBN);
        
        vec3 precalc = TBN * v_position;
        precalc.xy += i*vec2(1, 1) * 1000.;
        
        vec3 noise_val = texture(u_noise, precalc.xy*noise_frequency).rgb*noise_amplitude;
        
        vec3 world_dir = normalize(v_position - viewpos.xyz);
        vec3 dir = transpose(invTBN) * world_dir;
        
        float angle = atan(dir.y, dir.x);
        
        float dist;
        {
            
            vec4 ray = texture(u_raycast, vec3(precalc.xy - new_normal.xz, angle/2/3.141));
            
            normal = normalize(transpose(TBN) * ray.gba);
            dist = 1/(ray.r + 0.00001) - 1;
            
        }
        
        float flatdir = length(dir.xy);
        
        float depth = dist * abs(dir.z) / flatdir;
        
        {
            
            vec3 color = vec3(0, 1, 0);
            
            const vec3 lightdir = normalize(vec3(0.5,-1.0,0.5));
            
            float light = min(abs(dot(normal, lightdir)) * (1. - depth/grass_height/tile_size), 1.0);
            color = color * (light*0.7 + 0.3);
            
            vec4 v_clip_coord = viewproj * vec4(v_position + world_dir * (dist / flatdir * tile_size), 1.0);
            float f_ndc_depth = v_clip_coord.z / (v_clip_coord.w);
            
            if(f_ndc_depth < final_depth) {
                final_depth = f_ndc_depth;
                outColor = vec4(color, 1.0);
            }
            
        }
        
    }
    
    gl_FragDepth = final_depth;

}

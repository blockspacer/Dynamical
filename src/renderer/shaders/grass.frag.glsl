#version 450

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_uv;

layout(location = 0) out vec4 outColor;

layout(constant_id = 0) const int grass_height = 0;
layout(constant_id = 1) const int tile_size = 0;

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
    
    //if(-normal.y * normal.y * normal.y > 0.5) {
        
        vec3 new_normal = normalize(vec3(5*sin((v_position.x+v_position.z)/10.), 0.2, cos((v_position.x-v_position.z)/10.)));
        
        
        mat3 TBN = mat3(
            1., 0, new_normal.x,
            0,  0, new_normal.y,
            0, 1., new_normal.z
        );
        
        
        vec3 precalc = TBN * v_position;
        
        vec3 dir = precalc - TBN * viewpos.xyz;
        float viewdepth = length(dir);
        dir /= viewdepth;
        
        float angle = atan(dir.y, dir.x);
        
        vec4 ray = texture(u_raycast, vec3(precalc.xy/tile_size, angle/2/3.141));
        
        float flatdir = length(dir.xy);
        
        float depth = (1/ray.r - 1) * -dir.z / flatdir;
        
        if(depth < grass_height + 1.) {
            
            normal = ray.gba;
            
            outColor.rgb = vec3(0, 0.7, 0);
            
            float light = max(dot(normal, vec3(0.5,-1.,0.5)), 0.);
            outColor.rgb = outColor.rgb * (light*0.7 + 0.3);
            
            
            vec4 v_clip_coord = viewproj * vec4(inverse(TBN) * (precalc + dir * (1/ray.r - 1) / flatdir), 1.0);
            float f_ndc_depth = v_clip_coord.z / v_clip_coord.w;
            gl_FragDepth = f_ndc_depth;
            
            
            return;
        }
        
    //}
    
    discard;
    
}

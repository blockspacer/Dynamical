#version 450

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_uv;

layout(location = 0) out vec4 outColor;

layout(set = 1, binding = 0) uniform sampler2DArray u_color;

layout(constant_id = 0) const int chunk_size = 8*8*8;

layout(std140, set = 0, binding = 0) uniform UBO {
    mat4 viewproj;
    vec4 viewpos;
};

layout( push_constant ) uniform Args {
    vec3 color;
};

void main() {
    
    /*
    float separator = min(length(1.0 - min(abs((ivec3(v_position.xyz)%chunk_size)/2.), 1.0)) * 1000., 1.0);
    vec3 spos = v_position/length(v_position);
    outColor = vec4(abs(spos.x), separator, abs(spos.z), 1);
    */
    
    //vec3 normal = normalize(cross(dFdx(v_position), dFdy(v_position)));
    
    vec3 normal = normalize(v_normal);
    
    vec4 rock_color = max(mix(
            texture(u_color, vec3(v_position.yx/3., 1)),
            texture(u_color, vec3(v_position.yz/3., 1)),
            (normal.x * normal.x) / (normal.x * normal.x + normal.z * normal.z)
        ), 0);
    
    outColor = mix(
        rock_color,
        texture(u_color, vec3(v_position.xz/3., 0)),
        clamp(-normal.y * normal.y * normal.y, 0, 1)
    );
    
    
    float light = max(dot(normal, vec3(0.5,-1.,0.5)), 0.4);
    outColor.rgb = outColor.rgb * (light*0.7 + 0.3) + color;
    
}

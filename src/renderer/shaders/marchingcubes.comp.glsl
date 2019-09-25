#version 450

#extension GL_KHR_shader_subgroup_arithmetic: enable
#extension GL_KHR_shader_subgroup_ballot: enable

layout(local_size_x_id = 0) in;
layout(local_size_y_id = 1) in;
layout(local_size_z_id = 2) in;

const ivec3 offsets[24] = {
    // 0
    ivec3(0,0,0),
    ivec3(1,0,0),
    // 1
    ivec3(1,0,0),
    ivec3(1,0,1),
    // 2
    ivec3(1,0,1),
    ivec3(0,0,1),
    // 3
    ivec3(0,0,1),
    ivec3(0,0,0),
    // 4
    ivec3(0,1,0),
    ivec3(1,1,0),
    // 5
    ivec3(1,1,0),
    ivec3(1,1,1),
    // 6
    ivec3(1,1,1),
    ivec3(0,1,1),
    // 7
    ivec3(0,1,1),
    ivec3(0,1,0),
    // 8
    ivec3(0,0,0),
    ivec3(0,1,0),
    // 9
    ivec3(1,0,0),
    ivec3(1,1,0),
    // 10
    ivec3(1,0,1),
    ivec3(1,1,1),
    // 11
    ivec3(0,0,1),
    ivec3(0,1,1)
};

const uint trinum[256] = {0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 2, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 3, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 3, 2, 3, 3, 2, 3, 4, 4, 3, 3, 4, 4, 3, 4, 5, 5, 2, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 3, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 4, 2, 3, 3, 4, 3, 4, 2, 3, 3, 4, 4, 5, 4, 5, 3, 2, 3, 4, 4, 3, 4, 5, 3, 2, 4, 5, 5, 4, 5, 2, 4, 1, 1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 3, 2, 3, 3, 4, 3, 4, 4, 5, 3, 2, 4, 3, 4, 3, 5, 2, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 4, 3, 4, 4, 3, 4, 5, 5, 4, 4, 3, 5, 2, 5, 4, 2, 1, 2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 2, 3, 3, 2, 3, 4, 4, 5, 4, 5, 5, 2, 4, 3, 5, 4, 3, 2, 4, 1, 3, 4, 4, 5, 4, 5, 3, 4, 4, 5, 5, 2, 3, 4, 2, 1, 2, 3, 3, 2, 3, 4, 2, 1, 3, 2, 4, 1, 2, 1, 1, 0};

layout(set = 0, binding = 0) uniform isamplerBuffer triTable;

layout(std430, set=1, binding = 0) writeonly buffer Tris {
    vec4 tril[];
};

layout(std430, set=1, binding = 1) coherent buffer IndirectDraw {
    uint vertexCount;
    uint instanceCount;
    uint firstVertex;
    uint firstInstance;
};

layout (constant_id = 3) const uint chunk_size_x = 1;
layout (constant_id = 4) const uint chunk_size_y = 1;
layout (constant_id = 5) const uint chunk_size_z = 1;

layout(std430, set=1, binding = 2) readonly buffer Values {
    float val[chunk_size_x * chunk_size_y * chunk_size_z];
};

layout( push_constant ) uniform Args {
    ivec3 pos;
    int size;
    float time;
};

float sq(float x) {
    return x*x;
}

const float k = 100.;


float sdf2(vec3 p) {
    return 10. - p.y - 3*sin((p.x+p.z)/20.) - 5*cos((float(p.x)-p.z)/20.);
}

float sdf(vec3 p) {
    return sdf2(p);
}

float values(int x, int y, int z) {
    return val[(x+1) * chunk_size_y * chunk_size_z + (z+1) * chunk_size_y + y+1];
}

float values(ivec3 p) {
    return values(p.x, p.y, p.z);
}

float values(uint x, uint y, uint z) {
    return values(int(x), int(y), int(z));
}

void main() {
    
    if(gl_GlobalInvocationID.x == 0 && gl_GlobalInvocationID.y == 0 && gl_GlobalInvocationID.z == 0) {
        vertexCount = 0;
        instanceCount = 1;
        firstVertex = 0;
        firstInstance = 0;
    }
    barrier();
    
    uint val =    (values(gl_GlobalInvocationID.x  , gl_GlobalInvocationID.y  , gl_GlobalInvocationID.z  )>0 ? 1:0)
                | (values(gl_GlobalInvocationID.x+1, gl_GlobalInvocationID.y  , gl_GlobalInvocationID.z  )>0 ? 2:0)
                | (values(gl_GlobalInvocationID.x+1, gl_GlobalInvocationID.y  , gl_GlobalInvocationID.z+1)>0 ? 4:0)
                | (values(gl_GlobalInvocationID.x  , gl_GlobalInvocationID.y  , gl_GlobalInvocationID.z+1)>0 ? 8:0)
                | (values(gl_GlobalInvocationID.x  , gl_GlobalInvocationID.y+1, gl_GlobalInvocationID.z  )>0 ? 16:0)
                | (values(gl_GlobalInvocationID.x+1, gl_GlobalInvocationID.y+1, gl_GlobalInvocationID.z  )>0 ? 32:0)
                | (values(gl_GlobalInvocationID.x+1, gl_GlobalInvocationID.y+1, gl_GlobalInvocationID.z+1)>0 ? 64:0)
                | (values(gl_GlobalInvocationID.x  , gl_GlobalInvocationID.y+1, gl_GlobalInvocationID.z+1)>0 ? 128:0);
    
    uint num = trinum[val];
    if(num > 0) {
    
        uint local_tri_index = subgroupExclusiveAdd(num*3);
        
        // Find out which active invocation has the highest ID
        uint highestActiveID = subgroupBallotFindMSB(subgroupBallot(true));

        uint global_tri_index = 0;

        // If we're the highest active ID
        if (highestActiveID == gl_SubgroupInvocationID) {
            // We need to carve out a slice of out_triangles for our triangle
            global_tri_index = atomicAdd(vertexCount, local_tri_index + num*3);
        }

        global_tri_index = subgroupMax(global_tri_index);
        
        for(int i = 0; i<num*3; i++) {
            int edge = texelFetch(triTable, int(val) * 16 + i).r * 2;
            ivec3 a1 = ivec3(gl_GlobalInvocationID.xyz) + offsets[edge], a2 = ivec3(gl_GlobalInvocationID.xyz) + offsets[edge+1];
            float density1 = values(a1);
            float density2 = values(a2);
            vec3 vertex = mix(a1, a2, (-density1)/(density2 - density1)) * size + pos;
            
            vec3 norm1 = vec3(
            values(a1.x-1, a1.y, a1.z) - values(a1.x+1, a1.y, a1.z),
            values(a1.x, a1.y-1, a1.z) - values(a1.x, a1.y+1, a1.z),
            values(a1.x, a1.y, a1.z-1) - values(a1.x, a1.y, a1.z+1)
            );
            
            vec3 norm2 = vec3(
            values(a2.x-1, a2.y, a2.z) - values(a2.x+1, a2.y, a2.z),
            values(a2.x, a2.y-1, a2.z) - values(a2.x, a2.y+1, a2.z),
            values(a2.x, a2.y, a2.z-1) - values(a2.x, a2.y, a2.z+1)
            );
            
            vec3 normal = normalize(-mix(norm1, norm2, (-density1)/(density2 - density1)));
            
            vec2 uv = vec2(vertex.xz/32.);
            
            tril[(global_tri_index + local_tri_index+i)*2] = vec4(vertex, normal.x);
            tril[(global_tri_index + local_tri_index+i)*2 + 1] = vec4(normal.yz, uv);
        }
        
    }
    
}

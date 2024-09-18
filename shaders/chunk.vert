#version 460 core
layout (location = 0) in ivec3 a_position;
layout (location = 1) in int a_voxelId;
layout (location = 2) in int a_faceId;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

out vec3 vertexColor;
out vec2 vertexUV;

const vec2 uvCoords[4] = vec2[4](
        vec2(1, 0), vec2(1, 1),
        vec2(0, 0), vec2(0, 1)
);

const int uvIndices[12] = int[12](
        1, 0, 2, 1, 2, 3,  // even face
        3, 0, 2, 3, 1, 0   // odd face
);

vec3 hash31(float p) {
    vec3 p3 = fract(vec3(p * 21.2f) * vec3(0.1031f, 0.1030f, 0.0973f));
    p3 += dot(p3, p3.yzx + 33.33f);
    return fract((p3.xxy + p3.yzz) * p3.zyx) + 0.05;
}

void main() {
    int uvIndex = gl_VertexID % 6 + (a_faceId & 1) * 6;
    vertexUV = uvCoords[uvIndices[uvIndex]];

    vertexColor = hash31(a_voxelId);
    gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.f);
}

#version 460 core
layout (location = 0) in ivec3 a_position;
layout (location = 1) in int a_blockId;
layout (location = 2) in int a_faceId;
layout (location = 3) in int a_aoId;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

out vec3 vertexColor;
out vec2 vertexUV;
out float vertexShading;

const float aoValues[4] = float[4](
        0.1f, 0.25f, 0.5f, 1.f
);

const float faceShading[6] = float[6](
        1.f, 0.5f,   // top, bottom
        0.5f, 0.8f,  // right, left
        0.5f, 0.8f   // front, back
);

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
    return fract((p3.xxy + p3.yzz) * p3.zyx) + 0.05f;
}

void main() {
    int uvIndex = gl_VertexID % 6 + (a_faceId & 1) * 6;
    vertexUV = uvCoords[uvIndices[uvIndex]];

    vertexShading = faceShading[a_faceId] * aoValues[a_aoId];

    vertexColor = hash31(a_blockId);
    gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.f);
}

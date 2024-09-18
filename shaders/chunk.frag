#version 460 core

in vec3 vertexColor;
in vec2 vertexUV;

out vec4 fragmentColor;

const vec3 gamma = vec3(2.2f);
const vec3 inverseGamma = 1.f / gamma;

void main() {
    fragmentColor = vec4(vertexColor, 1.f);
}

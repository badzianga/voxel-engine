#version 460 core

in vec3 vertexColor;
in vec2 vertexUV;
in float vertexShading;

out vec4 fragmentColor;

uniform sampler2D u_texture;

const vec3 gamma = vec3(2.2f);
const vec3 inverseGamma = 1.f / gamma;

void main() {
    vec3 textureColor = texture(u_texture, vertexUV).rgb;

    textureColor = pow(textureColor, gamma);
//    textureColor.rgb *= vertexColor.rgb;

    textureColor.rgb = vec3(1.f);
    textureColor.rgb *= vertexShading;

    textureColor = pow(textureColor, inverseGamma);
    fragmentColor = vec4(textureColor, 1.f);
}

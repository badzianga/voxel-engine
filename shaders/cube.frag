#version 460 core

in vec3 vertexNormal;
in vec3 fragmentPosition;

out vec4 fragmentColor;

const vec3 objectColor = vec3(1.f, 0.25f, 0.5f);
const vec3 lightDirection = normalize(-vec3(0.2f, -1.0f, -0.3f));
const vec3 lightColor = vec3(1.f);

void main() {
    const float ambientStrenght = 0.1f;
    vec3 ambient = ambientStrenght * lightColor;

    vec3 normal = normalize(vertexNormal);
    float diff = max(dot(normal, lightDirection), 0.f);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * objectColor;
    fragmentColor = vec4(result, 1.f);
}

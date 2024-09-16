#version 460 core
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_normal;

out vec3 vertexNormal;
out vec3 fragmentPosition;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main() {
    vertexNormal = mat3(transpose(inverse(u_model))) * a_normal;
    fragmentPosition = vec3(u_model * vec4(a_position, 1.f));

    gl_Position = u_projection * u_view * vec4(fragmentPosition, 1.f);
}

#version 460 core
layout (location = 0) in vec3 a_position;
layout (location = 1) in vec3 a_color;

out vec3 vertexColor;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main() {
    vertexColor = a_color;

    gl_Position = u_projection * u_view * u_model * vec4(a_position, 1.f);
}

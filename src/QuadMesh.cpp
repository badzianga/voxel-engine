#include "QuadMesh.hpp"
#include <glad/glad.h>

QuadMesh::QuadMesh() : m_vao(0), m_vbo(0) {
    const float vertices[] = {
            -0.5f, -0.5f, 0.f, 1.f, 0.f, 0.f,
            0.5f, -0.5f, 0.f, 0.f, 0.f, 1.f,
            0.5f, 0.5f, 0.f, 0.f, 1.f, 0.f,
            0.5f, 0.5f, 0.f, 0.f, 1.f, 0.f,
            -0.5f, 0.5f, 0.f, 1.f, 1.f, 0.f,
            -0.5f, -0.5f, 0.f, 1.f, 0.f, 0.f,
    };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

QuadMesh::~QuadMesh() {
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

void QuadMesh::render() const {
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

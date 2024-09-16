#pragma once
#include <cstdint>

class QuadMesh {
public:
    QuadMesh();
    ~QuadMesh();
    void render() const;
private:
    uint32_t m_vao;
    uint32_t m_vbo;
};

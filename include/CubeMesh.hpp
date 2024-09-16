#pragma once
#include <cstdint>

class CubeMesh {
public:
    CubeMesh();
    ~CubeMesh();
    void render() const;
private:
    uint32_t m_vao;
    uint32_t m_vbo;
};

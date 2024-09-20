#pragma once
#include "Vertex.hpp"
#include <vector>

class Chunk {
public:
    Chunk();
    ~Chunk();
    void render() const;

    static const int size = 16;
    static const int height = 128;
    static const int area = size * size;
    static const int volume = area * height;
private:
    int addFace(int index, Vertex v0, Vertex v1, Vertex v2, Vertex v3, Vertex v4, Vertex v5);

    uint32_t m_vao;
    uint32_t m_vbo;
    std::vector<Vertex> m_vertices;
    int m_verticesCount;
};

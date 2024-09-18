#pragma once
#include "Vertex.hpp"
#include <vector>

class ChunkMesh {
public:
    ChunkMesh();
    ~ChunkMesh();
    void render() const;
private:
    int addData(int index, Vertex v0, Vertex v1, Vertex v2, Vertex v3, Vertex v4, Vertex v5);

    uint32_t m_vao;
    uint32_t m_vbo;
    std::vector<Vertex> m_vertices;
    int m_verticesCount;
};

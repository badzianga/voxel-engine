#pragma once
#include "Vertex.hpp"
#include <vector>

class Chunk {
public:
    Chunk(glm::ivec2 position);
    ~Chunk();
    void render() const;

    static const int size = 16;
    static const int height = 128;
    static const int area = size * size;
    static const int volume = area * height;
private:
    void generate();
    void buildMesh();
    int addFace(int index, Vertex v0, Vertex v1, Vertex v2, Vertex v3, Vertex v4, Vertex v5);

    uint32_t m_vao;
    uint32_t m_vbo;
    glm::ivec2 m_position;  // currently unused
    std::array<BlockId, Chunk::volume> m_blocks;
    std::vector<Vertex> m_vertices;
    int m_verticesCount;
};

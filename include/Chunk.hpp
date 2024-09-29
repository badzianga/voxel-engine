#pragma once
#include "Vertex.hpp"
#include "ChunkId.hpp"
#include <vector>

class Shader;

class Chunk {
public:
    explicit Chunk(glm::ivec2 position);
    ~Chunk();
    void generate();
    void buildMesh(const std::unordered_map<ChunkId, Chunk*>& chunks);
    void render(Shader& shader) const;
    [[nodiscard]] BlockId blockAt(int x, int y, int z) const;
    static const int size = 16;
    static const int height = 128;
    static const int area = size * size;
    static const int volume = area * height;
private:
    int addFace(int index, Vertex v0, Vertex v1, Vertex v2, Vertex v3, Vertex v4, Vertex v5);

    uint32_t m_vao;
    uint32_t m_vbo;
    glm::ivec2 m_position;
    glm::mat4 m_model;
    std::array<BlockId, Chunk::volume> m_blocks;
    std::vector<Vertex> m_vertices;
    int m_verticesCount;
};

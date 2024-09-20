#include "Chunk.hpp"
#include "BlockId.hpp"
#include <glad/glad.h>
#include <glm/gtc/noise.hpp>

constexpr int MAX_VISIBLE_VERTICES = 18;

bool isVoid(glm::u8vec3 blockPos, const std::vector<uint8_t>& chunkBlocks) {
    int x = blockPos.x;
    int y = blockPos.y;
    int z = blockPos.z;
    if (x >= 0 and x < Chunk::size and y >= 0 and y < Chunk::height and z >= 0 and z < Chunk::size) {
        if (chunkBlocks[x + Chunk::size * z + Chunk::area * y] > 0) {
            return false;
        }
    }
    return true;
}

Chunk::Chunk() : m_vao(0), m_vbo(0) {
    // TODO: temporary chunk data generation, will be in separate class in the future
    std::vector<uint8_t> chunkBlocks(Chunk::volume, 1);
    for (int y = 0; y < Chunk::height; ++y) {
        for (int z = 0; z < Chunk::size; ++z) {
            for (int x = 0; x < Chunk::size; ++x) {
                if (glm::simplex(glm::vec3(x, y, z) * 0.1f) + 1.f >= 1.f)
                    chunkBlocks[x + Chunk::size * z + Chunk::area * y] = 1 + x + y + z;
                else
                    chunkBlocks[x + Chunk::size * z + Chunk::area * y] = 0;
            }
        }
    }

    m_vertices.reserve(Chunk::volume * MAX_VISIBLE_VERTICES);

    int index = 0;

    for (int y = 0; y < Chunk::height; ++y) {
        for (int z = 0; z < Chunk::size; ++z) {
            for (int x = 0; x < Chunk::size; ++x) {
                BlockId blockId = chunkBlocks[x + Chunk::size * z + Chunk::area * y];
                // air - skip
                if (blockId == 0) {
                    continue;
                }

                // top face
                if (isVoid({x, y + 1, z}, chunkBlocks)) {
                    Vertex v0 = {{x, y + 1, z}, blockId, 0};
                    Vertex v1 = {{x + 1, y + 1, z}, blockId, 0};
                    Vertex v2 = {{x + 1, y + 1, z + 1}, blockId, 0};
                    Vertex v3 = {{x, y + 1, z + 1}, blockId, 0};

                    index = addFace(index, v0, v3, v2, v0, v2, v1);
                }

                // bottom face
                if (isVoid({x, y - 1, z}, chunkBlocks)) {
                    Vertex v0 = {{x, y, z}, blockId, 1};
                    Vertex v1 = {{x + 1, y, z}, blockId, 1};
                    Vertex v2 = {{x + 1, y, z + 1}, blockId, 1};
                    Vertex v3 = {{x, y, z + 1}, blockId, 1};

                    index = addFace(index, v0, v2, v3, v0, v1, v2);
                }

                // right face
                if (isVoid({x + 1, y, z}, chunkBlocks)) {
                    Vertex v0 = {{x + 1, y, z}, blockId, 2};
                    Vertex v1 = {{x + 1, y + 1, z}, blockId, 2};
                    Vertex v2 = {{x + 1, y + 1, z + 1}, blockId, 2};
                    Vertex v3 = {{x + 1, y, z + 1}, blockId, 2};

                    index = addFace(index, v0, v1, v2, v0, v2, v3);
                }

                // left face
                if (isVoid({x - 1, y, z}, chunkBlocks)) {
                    Vertex v0 = {{x, y, z}, blockId, 3};
                    Vertex v1 = {{x, y + 1, z}, blockId, 3};
                    Vertex v2 = {{x, y + 1, z + 1}, blockId, 3};
                    Vertex v3 = {{x, y, z + 1}, blockId, 3};

                    index = addFace(index, v0, v2, v1, v0, v3, v2);
                }

                // back face
                if (isVoid({x, y, z - 1}, chunkBlocks)) {
                    Vertex v0 = {{x, y, z}, blockId, 4};
                    Vertex v1 = {{x, y + 1, z}, blockId, 4};
                    Vertex v2 = {{x + 1, y + 1, z}, blockId, 4};
                    Vertex v3 = {{x + 1, y, z}, blockId, 4};

                    index = addFace(index, v0, v1, v2, v0, v2, v3);
                }

                // front face
                if (isVoid({x, y, z + 1}, chunkBlocks)) {
                    Vertex v0 = {{x, y, z + 1}, blockId, 5};
                    Vertex v1 = {{x, y + 1, z + 1}, blockId, 5};
                    Vertex v2 = {{x + 1, y + 1, z + 1}, blockId, 5};
                    Vertex v3 = {{x + 1, y, z + 1}, blockId, 5};

                    index = addFace(index, v0, v2, v1, v0, v3, v2);
                }
            }
        }
    }

    m_verticesCount = index;

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(m_verticesCount * sizeof(Vertex)), &m_vertices.front(), GL_STATIC_DRAW);

    glVertexAttribIPointer(0, 3, GL_UNSIGNED_BYTE, sizeof(Vertex), (const void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribIPointer(1, 1, GL_UNSIGNED_BYTE, sizeof(Vertex), (const void*)offsetof(Vertex, blockId));
    glEnableVertexAttribArray(1);

    glVertexAttribIPointer(2, 1, GL_UNSIGNED_BYTE, sizeof(Vertex), (const void*)offsetof(Vertex, faceId));
    glEnableVertexAttribArray(2);
}

Chunk::~Chunk() {
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

void Chunk::render() const {
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, m_verticesCount);
}

int Chunk::addFace(int index, Vertex v0, Vertex v1, Vertex v2, Vertex v3, Vertex v4, Vertex v5) {
    m_vertices[index++] = v0;
    m_vertices[index++] = v1;
    m_vertices[index++] = v2;
    m_vertices[index++] = v3;
    m_vertices[index++] = v4;
    m_vertices[index++] = v5;
    return index;
}

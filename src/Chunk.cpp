#include "Chunk.hpp"
#include "BlockId.hpp"
#include "Shader.hpp"
#include <glad/glad.h>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <utility>

constexpr int MAX_VISIBLE_VERTICES = 18;

static bool isVoid(glm::u8vec3 blockPos, const std::array<BlockId, Chunk::volume>& blocks) {
    int x = blockPos.x;
    int y = blockPos.y;
    int z = blockPos.z;
    if (x >= 0 and x < Chunk::size and y >= 0 and y < Chunk::height and z >= 0 and z < Chunk::size) {
        if (blocks[x + Chunk::size * z + Chunk::area * y] > 0) {
            return false;
        }
    }
    return true;
}

Chunk::Chunk() : m_vao(0), m_vbo(0), m_position(0), m_verticesCount(0), m_blocks(), m_model(1.f) {
    generate();
}

Chunk::Chunk(glm::ivec2 position) : m_vao(0), m_vbo(0), m_position(position), m_verticesCount(0), m_blocks(), m_model(1.f) {
    generate();
    m_model = glm::translate(m_model, glm::vec3(position.x, 0.f, position.y) * static_cast<float>(Chunk::size));
}

Chunk::~Chunk() {
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

Chunk& Chunk::operator=(Chunk&& other) noexcept {
    m_vao = std::exchange(other.m_vao, 0);
    m_vbo = std::exchange(other.m_vbo, 0);
    m_position = other.m_position;
    m_model = std::move(other.m_model);
    m_blocks = std::move(other.m_blocks);
    m_vertices = std::move(other.m_vertices);
    m_verticesCount = other.m_verticesCount;

    return *this;
}

void Chunk::setPosition(glm::ivec2 position) {
    m_position = position;
    m_model = glm::translate(glm::mat4{1.f}, glm::vec3(position.x, 0.f, position.y) * static_cast<float>(Chunk::size));
}

void Chunk::generate() {
    for (int y = 0; y < Chunk::height; ++y) {
        for (int z = 0; z < Chunk::size; ++z) {
            for (int x = 0; x < Chunk::size; ++x) {
//                const int worldX = x + m_position.x * Chunk::size;
//                const int worldZ = z + m_position.y * Chunk::size;
//                const glm::vec2 worldPos{ worldX, worldZ };
//                auto localHeight = int(glm::simplex(worldPos * 0.01f) * 32 + 64);
                int localHeight = 64;

                // TODO: maybe for loop order like z > x > y would be better
                // now, lots of localHeights are calculated unnecessarily

                if (y < localHeight) {
                    m_blocks[x + Chunk::size * z + Chunk::area * y] = 1 + y;
                }
            }
        }
    }
}

void Chunk::buildMesh() {
    m_vertices.reserve(Chunk::volume * MAX_VISIBLE_VERTICES);

    int index = 0;

    for (int y = 0; y < Chunk::height; ++y) {
        for (int z = 0; z < Chunk::size; ++z) {
            for (int x = 0; x < Chunk::size; ++x) {
                BlockId blockId = m_blocks[x + Chunk::size * z + Chunk::area * y];
                // air - skip
                if (blockId == 0) {
                    continue;
                }

                // top face
                if (isVoid({x, y + 1, z}, m_blocks)) {
                    Vertex v0 = {{x, y + 1, z}, blockId, 0};
                    Vertex v1 = {{x + 1, y + 1, z}, blockId, 0};
                    Vertex v2 = {{x + 1, y + 1, z + 1}, blockId, 0};
                    Vertex v3 = {{x, y + 1, z + 1}, blockId, 0};

                    index = addFace(index, v0, v3, v2, v0, v2, v1);
                }

                // bottom face
                if (isVoid({x, y - 1, z}, m_blocks)) {
                    Vertex v0 = {{x, y, z}, blockId, 1};
                    Vertex v1 = {{x + 1, y, z}, blockId, 1};
                    Vertex v2 = {{x + 1, y, z + 1}, blockId, 1};
                    Vertex v3 = {{x, y, z + 1}, blockId, 1};

                    index = addFace(index, v0, v2, v3, v0, v1, v2);
                }

                // right face
                if (isVoid({x + 1, y, z}, m_blocks)) {
                    Vertex v0 = {{x + 1, y, z}, blockId, 2};
                    Vertex v1 = {{x + 1, y + 1, z}, blockId, 2};
                    Vertex v2 = {{x + 1, y + 1, z + 1}, blockId, 2};
                    Vertex v3 = {{x + 1, y, z + 1}, blockId, 2};

                    index = addFace(index, v0, v1, v2, v0, v2, v3);
                }

                // left face
                if (isVoid({x - 1, y, z}, m_blocks)) {
                    Vertex v0 = {{x, y, z}, blockId, 3};
                    Vertex v1 = {{x, y + 1, z}, blockId, 3};
                    Vertex v2 = {{x, y + 1, z + 1}, blockId, 3};
                    Vertex v3 = {{x, y, z + 1}, blockId, 3};

                    index = addFace(index, v0, v2, v1, v0, v3, v2);
                }

                // back face
                if (isVoid({x, y, z - 1}, m_blocks)) {
                    Vertex v0 = {{x, y, z}, blockId, 4};
                    Vertex v1 = {{x, y + 1, z}, blockId, 4};
                    Vertex v2 = {{x + 1, y + 1, z}, blockId, 4};
                    Vertex v3 = {{x + 1, y, z}, blockId, 4};

                    index = addFace(index, v0, v1, v2, v0, v2, v3);
                }

                // front face
                if (isVoid({x, y, z + 1}, m_blocks)) {
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

void Chunk::render(Shader& shader) const {
    shader.setMat4("u_model", m_model);
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

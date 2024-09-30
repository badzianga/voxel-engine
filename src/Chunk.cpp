#include "Chunk.hpp"
#include "BlockId.hpp"
#include "Shader.hpp"
#include <glad/glad.h>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/matrix_transform.hpp>

constexpr int MAX_VISIBLE_VERTICES = 18;

static int posMod(int i) {
    return (i % Chunk::size + Chunk::size) % Chunk::size;
}

static bool isVoid(const glm::ivec3& localPos, const glm::ivec3& worldPos, const std::unordered_map<ChunkId, Chunk*>& chunks) {
    int x = localPos.x;
    int y = localPos.y;
    int z = localPos.z;

    glm::ivec2 chunkPos{ (int)std::floor((float)worldPos.x / Chunk::size), (int)std::floor((float)worldPos.z / Chunk::size) };
//    LOG_DEBUG("Calculated chunk pos: (" + std::to_string(chunkPos.x) + ' ' + std::to_string(chunkPos.y) + ')');
    ChunkId chunkId = calculateChunkId(chunkPos);
    auto chunkIt = chunks.find(chunkId);
    if (chunkIt == chunks.end()) {
//        LOG_DEBUG("Chunk with position (" + std::to_string(chunkPos.x) + ' ' + std::to_string(chunkPos.y) + ") doesn't exist, so neighbor is void");
        return true;
    }

    glm::ivec3 blockPos{posMod(x), y, posMod(z)};
    if (chunkIt->second->blockAt(blockPos.x, blockPos.y, blockPos.z) != 0) {
        return false;
    }
    return true;
}

static std::array<uint8_t, 4> getAmbientOcclusion(const glm::ivec3& localPos, const glm::ivec3& worldPos, const std::unordered_map<ChunkId, Chunk*>& chunks, char plane) {
    assert(plane == 'X' or plane == 'Y' or plane == 'Z');

    int x = localPos.x;
    int y = localPos.y;
    int z = localPos.z;

    int wx = worldPos.x;
    int wy = worldPos.y;
    int wz = worldPos.z;

    bool a, b, c, d, e, f, g, h;

    // b a h
    // c   g
    // d e f
    // TODO: ambient occlusion on some corners works incorrectly (ignores b/d/f/h block), investigate it further
    if (plane == 'Y') {
        a = isVoid({x, y, z - 1}, {wx, wy, wz - 1}, chunks);
        b = isVoid({x - 1, y, z - 1}, {wx - 1, wy, wz - 1}, chunks);
        c = isVoid({x - 1, y, z}, {wx - 1, wy, wz}, chunks);
        d = isVoid({x - 1, y, z + 1}, {wx - 1, wy, wz + 1}, chunks);
        e = isVoid({x, y, z + 1}, {wx, wy, wz + 1}, chunks);
        f = isVoid({x + 1, y, z + 1}, {wx + 1, wy, wz + 1}, chunks);
        g = isVoid({x + 1, y, z}, {wx + 1, wy, wz}, chunks);
        h = isVoid({x + 1, y, z - 1}, {wx + 1, wy, wz - 1}, chunks);
    }
    else if (plane == 'X') {
        a = isVoid({x, y, z - 1}, {wx, wy, wz - 1}, chunks);
        b = isVoid({x, y - 1, z - 1}, {wx, wy - 1, wz - 1}, chunks);
        c = isVoid({x, y - 1, z}, {wx, wy - 1, wz}, chunks);
        d = isVoid({x, y - 1, z + 1}, {wx, wy - 1, wz + 1}, chunks);
        e = isVoid({x, y, z + 1}, {wx, wy, wz + 1}, chunks);
        f = isVoid({x, y + 1, z + 1}, {wx, wy + 1, wz + 1}, chunks);
        g = isVoid({x, y + 1, z}, {wx, wy + 1, wz}, chunks);
        h = isVoid({x, y + 1, z - 1}, {wx, wy + 1, wz - 1}, chunks);
    }
    else /* (plane == 'Z') */ {
        a = isVoid({x - 1, y, z}, {wx - 1, wy, wz}, chunks);
        b = isVoid({x - 1, y - 1, z}, {wx - 1, wy - 1, wz}, chunks);
        c = isVoid({x, y - 1, z}, {wx, wy - 1, wz}, chunks);
        d = isVoid({x + 1, y - 1, z}, {wx + 1, wy - 1, wz}, chunks);
        e = isVoid({x + 1, y, z}, {wx + 1, wy, wz}, chunks);
        f = isVoid({x + 1, y + 1, z}, {wx + 1, wy + 1, wz}, chunks);
        g = isVoid({x, y + 1, z}, {wx, wy + 1, wz}, chunks);
        h = isVoid({x - 1, y + 1, z}, {wx - 1, wy + 1, wz}, chunks);
    }

    const std::array<uint8_t, 4> ambientOcclusion = {
            static_cast<uint8_t>(a + b + c),
            static_cast<uint8_t>(g + h + a),
            static_cast<uint8_t>(e + f + g),
            static_cast<uint8_t>(c + d + e)
    };
    return ambientOcclusion;
}

Chunk::Chunk(glm::ivec2 position) : m_vao(0), m_vbo(0), m_position(position), m_verticesCount(0), m_blocks(), m_model(1.f) {
    generate();
    m_model = glm::translate(m_model, glm::vec3(position.x, 0.f, position.y) * static_cast<float>(Chunk::size));
//    LOG_DEBUG("Created chunk at position (" + std::to_string(m_position.x) + ' ' + std::to_string(m_position.y) + ')');
}

Chunk::~Chunk() {
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
//    LOG_DEBUG("Deleted chunk at position (" + std::to_string(m_position.x) + ' ' + std::to_string(m_position.y) + ')');
}

void Chunk::generate() {
    for (int z = 0; z < Chunk::size; ++z) {
        for (int x = 0; x < Chunk::size; ++x) {
            const int worldX = x + m_position.x * Chunk::size;
            const int worldZ = z + m_position.y * Chunk::size;
            const glm::vec2 worldPos{ worldX, worldZ };
            auto localHeight = int(glm::simplex(worldPos * 0.01f) * 32 + 64);
            for (int y = 0; y < localHeight; ++y) {
                if (y < localHeight) {
                    m_blocks[x + Chunk::size * z + Chunk::area * y] = 1 + y;
                }
            }
        }
    }
}

void Chunk::buildMesh(const std::unordered_map<ChunkId, Chunk*>& chunks) {
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

                int worldX = x + m_position.x * Chunk::size;
                int worldZ = z + m_position.y * Chunk::size;

                // top face
                if (isVoid({x, y + 1, z}, {worldX, y + 1, worldZ}, chunks)) {
                    auto ao = getAmbientOcclusion({x, y + 1, z}, {worldX, y + 1, worldZ}, chunks, 'Y');
                    uint8_t flipId = ao[1] + ao[3] > ao[0] + ao[2];

                    Vertex v0 = {{x, y + 1, z}, blockId, 0, ao[0], flipId};
                    Vertex v1 = {{x + 1, y + 1, z}, blockId, 0, ao[1], flipId};
                    Vertex v2 = {{x + 1, y + 1, z + 1}, blockId, 0, ao[2], flipId};
                    Vertex v3 = {{x, y + 1, z + 1}, blockId, 0, ao[3], flipId};

                    if (flipId)
                        index = addFace(index, v1, v0, v3, v1, v3, v2);
                    else
                        index = addFace(index, v0, v3, v2, v0, v2, v1);
                }

                // bottom face
                if (isVoid({x, y - 1, z}, {worldX, y - 1, worldZ}, chunks)) {
                    auto ao = getAmbientOcclusion({x, y - 1, z}, {worldX, y - 1, worldZ}, chunks, 'Y');
                    uint8_t flipId = ao[1] + ao[3] > ao[0] + ao[2];

                    Vertex v0 = {{x, y, z}, blockId, 1, ao[0], flipId};
                    Vertex v1 = {{x + 1, y, z}, blockId, 1, ao[1], flipId};
                    Vertex v2 = {{x + 1, y, z + 1}, blockId, 1, ao[2], flipId};
                    Vertex v3 = {{x, y, z + 1}, blockId, 1, ao[3], flipId};

                    if (flipId)
                        index = addFace(index, v1, v3, v0, v1, v2, v3);
                    else
                        index = addFace(index, v0, v2, v3, v0, v1, v2);
                }

                // right face
                if (isVoid({x + 1, y, z}, {worldX + 1, y, worldZ}, chunks)) {
                    auto ao = getAmbientOcclusion({x + 1, y, z}, {worldX + 1, y, worldZ}, chunks, 'X');
                    uint8_t flipId = ao[1] + ao[3] > ao[0] + ao[2];

                    Vertex v0 = {{x + 1, y, z}, blockId, 2, ao[0], flipId};
                    Vertex v1 = {{x + 1, y + 1, z}, blockId, 2, ao[1], flipId};
                    Vertex v2 = {{x + 1, y + 1, z + 1}, blockId, 2, ao[2], flipId};
                    Vertex v3 = {{x + 1, y, z + 1}, blockId, 2, ao[3], flipId};

                    if (flipId)
                        index = addFace(index, v3, v0, v1, v3, v1, v2);
                    else
                        index = addFace(index, v0, v1, v2, v0, v2, v3);
                }

                // left face
                if (isVoid({x - 1, y, z}, {worldX - 1, y, worldZ}, chunks)) {
                    auto ao = getAmbientOcclusion({x - 1, y, z}, {worldX - 1, y, worldZ}, chunks, 'X');
                    uint8_t flipId = ao[1] + ao[3] > ao[0] + ao[2];

                    Vertex v0 = {{x, y, z}, blockId, 3, ao[0], flipId};
                    Vertex v1 = {{x, y + 1, z}, blockId, 3, ao[1], flipId};
                    Vertex v2 = {{x, y + 1, z + 1}, blockId, 3, ao[2], flipId};
                    Vertex v3 = {{x, y, z + 1}, blockId, 3, ao[3], flipId};

                    if (flipId)
                        index = addFace(index, v3, v1, v0, v3, v2, v1);
                    else
                        index = addFace(index, v0, v2, v1, v0, v3, v2);
                }

                // back face
                if (isVoid({x, y, z - 1}, {worldX, y, worldZ - 1}, chunks)) {
                    auto ao = getAmbientOcclusion({x, y, z - 1}, {worldX, y, worldZ - 1}, chunks, 'Z');
                    uint8_t flipId = ao[1] + ao[3] > ao[0] + ao[2];

                    Vertex v0 = {{x, y, z}, blockId, 4, ao[0], flipId};
                    Vertex v1 = {{x, y + 1, z}, blockId, 4, ao[1], flipId};
                    Vertex v2 = {{x + 1, y + 1, z}, blockId, 4, ao[2], flipId};
                    Vertex v3 = {{x + 1, y, z}, blockId, 4, ao[3], flipId};

                    if (flipId)
                        index = addFace(index, v3, v0, v1, v3, v1, v2);
                    else
                        index = addFace(index, v0, v1, v2, v0, v2, v3);
                }

                // front face
                if (isVoid({x, y, z + 1}, {worldX, y, worldZ + 1}, chunks)) {
                    auto ao = getAmbientOcclusion({x, y, z + 1}, {worldX, y, worldZ + 1}, chunks, 'Z');
                    uint8_t flipId = ao[1] + ao[3] > ao[0] + ao[2];

                    Vertex v0 = {{x, y, z + 1}, blockId, 5, ao[0], flipId};
                    Vertex v1 = {{x, y + 1, z + 1}, blockId, 5, ao[1], flipId};
                    Vertex v2 = {{x + 1, y + 1, z + 1}, blockId, 5, ao[2], flipId};
                    Vertex v3 = {{x + 1, y, z + 1}, blockId, 5, ao[3], flipId};

                    if (flipId)
                        index = addFace(index, v3, v1, v0, v3, v2, v1);
                    else
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

    glVertexAttribIPointer(3, 1, GL_UNSIGNED_BYTE, sizeof(Vertex), (const void*)offsetof(Vertex, aoId));
    glEnableVertexAttribArray(3);

    glVertexAttribIPointer(4, 1, GL_UNSIGNED_BYTE, sizeof(Vertex), (const void*)offsetof(Vertex, flipId));
    glEnableVertexAttribArray(4);

//    LOG_DEBUG("Built mesh with " + std::to_string(m_verticesCount)
//        + " vertices for chunk at position (" + std::to_string(m_position.x) + ' ' + std::to_string(m_position.y) + ')');
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

BlockId Chunk::blockAt(int x, int y, int z) const {
    if (x < 0 or x >= Chunk::size or y < 0 or y >= Chunk::height or z < 0 or z >= Chunk::size) {
//        LOG_DEBUG("Trying to get block from chunk ("
//            + std::to_string(m_position.x) + ' ' + std::to_string(m_position.y)
//            + ") at position (" + std::to_string(x) + ' ' + std::to_string(y) + ' ' + std::to_string(z) + ')');
        return 0;
    }
    int index = x + Chunk::size * z + Chunk::area * y;
    return m_blocks[index];
}

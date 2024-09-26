#include <iostream>
#include "World.hpp"
#include "Chunk.hpp"

World::World() {
    for (int z = -8; z < 8; ++z) {
        for (int x = -8; x < 8; ++x) {
            glm::ivec2 chunkPos{ x, z };
            addChunk(chunkPos);
        }
    }
    for (auto& chunk : m_chunks) {
        chunk.second.buildMesh();
    }
}

void World::render(Shader& shader) const {
    for (auto& chunk : m_chunks) {
        chunk.second.render(shader);
    }
}

void World::addChunk(glm::ivec2 chunkPosition) {
    ChunkId chunkId = calculateChunkId(chunkPosition);
    m_chunks[chunkId] = Chunk(chunkPosition);
}

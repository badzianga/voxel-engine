#include <iostream>
#include "World.hpp"
#include "Chunk.hpp"

World::World() {
    for (int z = -2; z < 2; ++z) {
        for (int x = -2; x < 2; ++x) {
            glm::ivec2 chunkPos{ x, z };
            addChunk(chunkPos);
        }
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

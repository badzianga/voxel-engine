#include <iostream>
#include "World.hpp"
#include "Chunk.hpp"
#include "Logger.hpp"

World::World() {
    for (int z = -1; z < 2; ++z) {
        for (int x = -1; x < 2; ++x) {
            glm::ivec2 chunkPos{ x, z };
            addChunk(chunkPos);
        }
    }
    LOG_DEBUG("Created " + std::to_string(m_chunks.size()) + " chunks");
    for (auto& chunkPair : m_chunks) {
        chunkPair.second->buildMesh();
    }
}

World::~World() {
    for (auto& chunkPair : m_chunks) {
        delete chunkPair.second;
    }
    LOG_DEBUG("Deleted " + std::to_string(m_chunks.size()) + " chunks");
}

void World::render(Shader& shader) const {
    for (auto& chunkPair : m_chunks) {
        chunkPair.second->render(shader);
    }
}

void World::addChunk(glm::ivec2 chunkPosition) {
    ChunkId chunkId = calculateChunkId(chunkPosition);
    m_chunks[chunkId] = new Chunk(chunkPosition);
}

#include <iostream>
#include "World.hpp"
#include "Chunk.hpp"
#include "Logger.hpp"
#include "Timer.hpp"

World::World() {
    Timer timer{};
    timer.start();
    for (int z = -4; z < 4; ++z) {
        for (int x = -4; x < 4; ++x) {
            glm::ivec2 chunkPos{ x, z };
            addChunk(chunkPos);
        }
    }
    LOG_DEBUG("Generated " + std::to_string(m_chunks.size()) + " chunks after " + std::to_string(timer.getElapsedTime()) + 's');
    for (auto& chunkPair : m_chunks) {
        chunkPair.second->buildMesh(m_chunks);
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

#include <iostream>
#include "World.hpp"
#include "Chunk.hpp"
#include "Logger.hpp"
#include "Timer.hpp"

World::World() {
    Timer timer{};
    timer.start();
    for (int z = -8; z < 8; ++z) {
        for (int x = -8; x < 8; ++x) {
            glm::ivec2 chunkPos{ x, z };
            addChunk(chunkPos);
        }
    }
    LOG_DEBUG("Generated " + std::to_string(m_chunks.size()) + " chunks after " + std::to_string(timer.getElapsedTime()) + 's');
    timer.start();
    for (auto& chunkPair : m_chunks) {
        chunkPair.second->buildMesh(m_chunks);
    }
    LOG_DEBUG("Built chunk meshes after " + std::to_string(timer.getElapsedTime()) + 's');
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

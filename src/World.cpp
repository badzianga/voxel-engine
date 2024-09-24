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
}

World::~World() {
    for (auto& chunk : m_chunks) {
        delete chunk.second;
    }
}

void World::render(Shader& shader) const {
    for (auto& chunk : m_chunks) {
        chunk.second->render(shader);
    }
}

void World::addChunk(glm::ivec2 chunkPosition) {
    /* TODO: I think something bad might be happening here when using Chunk variables instead of pointers - like
     * generating same chunk two times. I should write proper copy/move constructors for Chunk and not use pointers */
    ChunkId chunkId = calculateChunkId(chunkPosition);
    std::cout << "ID: " << static_cast<int>(chunkId) << ' ';
    auto chunk = new Chunk(chunkPosition);
    m_chunks[chunkId] = chunk;
}

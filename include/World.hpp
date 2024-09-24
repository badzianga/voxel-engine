#pragma once
#include <glm/glm.hpp>
#include "Chunk.hpp"
#include "ChunkId.hpp"
#include <unordered_map>

class World {
public:
    World();
    ~World();
    void render(Shader& shader) const;
private:
    void addChunk(glm::ivec2 chunkPosition);

    std::unordered_map<ChunkId, Chunk*> m_chunks;
};

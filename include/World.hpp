#pragma once
#include <glm/glm.hpp>
#include "Chunk.hpp"
#include <vector>

class World {
public:
    World();
    ~World();
    void render(Shader& shader) const;
    static glm::ivec3 getCenter();

    static const int width = 8;
    static const int depth = 8;
    static const int area = width * depth;
private:
    std::vector<Chunk*> m_chunks;
};

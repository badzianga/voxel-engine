#include <iostream>
#include "World.hpp"
#include "Chunk.hpp"
#include "Timer.hpp"

World::World() {
    Timer timer{};
    timer.start();
    for (int z = 0; z < World::depth; ++z) {
        for (int x = 0; x < World::width; ++x) {
            // TODO: I think something bad might be happening here when using Chunk variables instead of pointers
            //  - like generating same chunk two times
            // I should write proper copy/move constructors for Chunk and get rid of pointers
            auto chunk = new Chunk(glm::ivec2{ x, z });
            m_chunks.push_back(chunk);
        }
    }
    std::cout << "Chunks generated after " << timer.getElapsedTime() << "s\n";
}

World::~World() {
    for (auto& chunk : m_chunks) {
        delete chunk;
    }
}

void World::render(Shader& shader) const {
    for (auto& chunk : m_chunks) {
        chunk->render(shader);
    }
}

glm::ivec3 World::getCenter() {
    int x = World::width * (Chunk::size / 2);
    int y = Chunk::height / 2 + 2;
    int z = World::depth * (Chunk::size / 2);
    return { x, y, z };
}

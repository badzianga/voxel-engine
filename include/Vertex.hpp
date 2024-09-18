#pragma once
#include <cstdint>
#include <glm/glm.hpp>

struct Vertex {
    glm::u8vec3 position;
    uint8_t voxelId;
    uint8_t faceId;
};

#pragma once
#include "BlockId.hpp"
#include <glm/glm.hpp>

struct Vertex {
    glm::u8vec3 position;
    BlockId blockId;
    uint8_t faceId;
    uint8_t aoId;
};

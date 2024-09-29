#pragma once
#include <cstdint>
#include <glm/glm.hpp>

using ChunkId = int32_t;

const int chunkIdTypeSizeInBits = sizeof(ChunkId) * 8;

ChunkId calculateChunkId(glm::ivec2 chunkPosition);

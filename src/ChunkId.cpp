#include "ChunkId.hpp"

// So, let's say max size of the world will be 16 x 16 chunks.
// With that, it's possible to store chunk id in 1 byte (8 bits)
// by using x and z coordinates as follows: z * 2^(bits/2) + x
// However, in that situation, min and max ids are -136 and 119.
// they have to be increased by 2^(bits-1), so the ids will be
// -128 and 127.

ChunkId calculateChunkId(glm::ivec2 chunkPosition) {
    int x = chunkPosition.x;
    int z = chunkPosition.y;
    return z * (1 << (chunkIdTypeSizeInBits >> 1)) + x    // main part (z * DEPTH_JUMP + x)
           + ((1 << (chunkIdTypeSizeInBits >> 1)) >> 1);  // offset part (DEPTH_JUMP / 2)
}

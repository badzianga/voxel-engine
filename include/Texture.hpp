#pragma once
#include <cstdint>
#include <glad/glad.h>

class Texture {
public:
    Texture();
    Texture(const char* filePath, bool flipY=false);
    ~Texture();
    void load(const char* filePath, bool flipY=false);
    void use() const;
private:
    uint32_t m_id;
};

#pragma once
#include "../util/string.h"

struct Texture
{
    unsigned int id;
    String filepath;
    bool valid;
    unsigned int widthInPixels;
    unsigned int heightInPixels;

public:
    Texture() = delete;
    ~Texture();
    void del();
    void bind(unsigned int slot = 0) const;
    static Texture* loadTexture(const String& filepath);
};

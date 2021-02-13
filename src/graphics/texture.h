#pragma once
#include <string>

struct Texture
{
    unsigned int id;
    std::string filepath;
    bool valid;
    unsigned int widthInPixels;
    unsigned int heightInPixels;

public:
    Texture();
    ~Texture();
    void bind(unsigned int slot = 0) const;
    static Texture* loadTexture(const std::string& filepath);
private:
    Texture(unsigned int id, const std::string& filepath);
};

#pragma once
#include <string>

class Texture
{
    unsigned int _id;
    int _index;
    bool _valid;

public:
    Texture();
    ~Texture();
    void bind(unsigned int slot = 0) const;
    static Texture* loadTexture(const std::string& filepath);
private:
    Texture(unsigned int id);
};
#pragma once
#include "../util/vector.h"
#include <string>

struct Texture;

struct Letter
{
    char character;
    float x, y;
    float width, height;
    float xOffset, yOffset;
    float xAdvance;
};

struct Font
{
    unsigned int _id;
    Texture* _atlas;
    std::string _filepath;
    Vector<Letter> _letters;
    float _base;
public:
    static Font* loadFont(std::string filepath);
    Letter* getLetter(char c);
private:
    Font(Vector<Letter> letters, Texture* atlas, float base, std::string filepath);
    static unsigned int nextId();
};

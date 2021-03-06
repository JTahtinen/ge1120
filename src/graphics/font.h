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
    unsigned int id;
    Texture* atlas;
    std::string filepath;
    Vector<Letter> letters;
    float base;
    

    Letter* getLetter(char c);
};

Font* loadFont(std::string filepath);
void deleteFont(Font* font);
unsigned int nextIDFont();

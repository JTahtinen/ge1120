#pragma once
#include "../util/vector.h"
#include "../util/string.h"
#include "../defs.h"
#include "../graphics/font.h"
#include "../math/vec4.h"

struct Button
{
    unsigned int id;
    String text;
    bool hot;
    bool active;
};

struct ButtonBox
{
    unsigned int id;
    bool active;
    String title;
    Vector<Button> buttons;
    Vec2 screenPos;
    unsigned int nextID;
    bool drag;
    Vec2 dragPoint;
    Vec4* currentHeaderColor;
    bool inFocus;
    Vec2 dimensions;
};

unsigned int createButton(const String& string, ButtonBox* box);
bool doButton(unsigned int buttonID, ButtonBox* box);
bool initButtonBox(const String& title, unsigned int id, ButtonBox* box);
void setButtonBoxActive(bool val, ButtonBox* box);
void updateButtonBox(ButtonBox* box);
void drawButtonBox(ButtonBox* box);
bool isCursorOnHeader(ButtonBox* box);
bool isCursorOnBox(ButtonBox* box);

inline float calculateStringWidth(String& text, Font* font, float scale = 0.2f)
{
    if (!font)
    {
        err("Could not calculate string width - font was NULL!\n");
        return 0;
    }
    float result = 0;
    for (unsigned int i = 0; i < text.size; ++i)
    {
        if (!text[i]) break;
        Letter* c = font->getLetter(text[i]);
        result += c->xAdvance * scale;
    }
    return result;
}

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

struct UI
{
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

unsigned int createButton(const String& string, UI* ui);
bool doButton(unsigned int buttonID, UI* ui);
bool initUI(const String& title, UI* ui);
void updateUI(UI* ui);
void drawUI(UI* ui);

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


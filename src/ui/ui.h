#pragma once
#include "../util/vector.h"
#include <string>
#include "../defs.h"
#include "../graphics/font.h"

struct Button
{
    unsigned int id;
    std::string text;
    bool hot;
    bool active;
};

struct UI
{
    std::string title;
    Vector<Button> buttons;
    Vec2 screenPos;
    unsigned int nextID;
};

unsigned int createButton(const std::string& string, UI* ui);
bool doButton(unsigned int buttonID, UI* ui);
bool initUI(const std::string& title, UI* ui);
//UI* createUI(const std::string& title);
void updateUI(UI* ui);
void drawUI(UI* ui);

inline float calculateStringWidth(std::string text, Font* font, float scale = 0.2f)
{
    if (!font)
    {
        err("Could not calculate string width - font was NULL!\n");
        return 0;
    }
    float result = 0;
    for (unsigned int i = 0; i < text.size(); ++i)
    {
        Letter* c = font->getLetter(text[i]);
        result += c->xAdvance * scale;
    }
    return result;
}


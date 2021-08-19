#pragma once
#include "buttonbox.h"
#include "../util/vector.h"
#include "../util/string.h"
struct UI
{
    unsigned int nextID;
    Vector<ButtonBox> buttonBoxes;
    int currentElementIndex;
};

void initUI(UI* ui);
ButtonBox* createButtonBox(const String& title, UI* ui);
void updateUI(UI* ui);
void drawUI(UI* ui);
bool cursorOnUI(UI* ui);

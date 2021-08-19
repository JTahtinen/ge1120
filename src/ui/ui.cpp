#include "ui.h"
#include "../defs.h"
#include "../context.h"

static unsigned int nextID;

void initUI(UI* ui)
{
    if (!ui)
    {
        err("Could not init UI. Pointer was NULL!\n");
    }
    ui->nextID = 1;
    ui->buttonBoxes.reserve(10);
    ui->currentElementIndex = -1;
}

ButtonBox* createButtonBox(const String& title, UI* ui)
{
    if (!ui) return 0;

    unsigned int id = ui->nextID++;
    ui->buttonBoxes.push_back({});
    ButtonBox& box = ui->buttonBoxes.back();
    initButtonBox(title, id, &box);
    return &box;
}

void updateUI(UI* ui)
{
    if (!ui) return;

    ButtonBox* selectedBox = NULL; 
    if (ui->currentElementIndex > -1)
    {
        selectedBox = &ui->buttonBoxes[ui->currentElementIndex];        
    }
    if (!selectedBox || (!selectedBox->drag && !isCursorOnBox(selectedBox)))
    {
        for (int i = ui->buttonBoxes.size - 1; i >= 0; --i)
        {
            ButtonBox& box = ui->buttonBoxes[i];
            if (box.active && isCursorOnBox(&box))
            {
                ui->currentElementIndex = i;
                selectedBox = &box;
                break;
            }
        }
    }
    if (selectedBox)
    {
        //if (isCursorOnBox(selectedBox) && g_input.mouseLeftClicked)
        //{
        //    g_screenContext = CONTEXT_UI;
        //}
        
        if (selectedBox->active && (g_screenContext == CONTEXT_UI || g_screenContext == CONTEXT_EVERYTHING))
        {
            updateButtonBox(selectedBox);
        }        
    }
}

void drawUI(UI* ui)
{
    if (!ui) return;
    
    for (int i = 0; i < ui->buttonBoxes.size; ++i)
    {
        if (i == ui->currentElementIndex) continue;
        ButtonBox& box = ui->buttonBoxes[i];       
        drawButtonBox(&box);
    }
    if (ui->currentElementIndex > -1)
    {
        drawButtonBox(&ui->buttonBoxes[ui->currentElementIndex]);
    }
}

bool cursorOnUI(UI* ui)
{
    if (!ui) return false;
    for (unsigned int i = 0; i < ui->buttonBoxes.size; ++i)
    {
        if (isCursorOnBox(&ui->buttonBoxes[i]))
        {
            return true;
        }
    }
    return false;
}

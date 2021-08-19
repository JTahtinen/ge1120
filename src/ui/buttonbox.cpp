#include "buttonbox.h"
#include "../defs.h"
#include "../globals.h"
#include "../math/math.h"
#include "../graphics/renderer.h"

static Vec4 buttonIdleColor(0.55f, 0.6f, 0.1f, 1.0f);
static Vec4 buttonHotColor(0.7f, 0.8f, 0.2f, 1.0f);
static Vec4 buttonActiveColor(1, 1, 1, 1);
static Vec4 headerIdleColor(0.5f, 0, 0, 0.5f);
static Vec4 headerHotColor(1.0f, 0, 0, 0.5f);
static Vec4 backgroundColor(0, 0, 0, 0.5f);
static float backgroundW = 0.25f;
static float buttonH = 0.04f;
static float headerH = 0.06f;
static float buttonXMargin = 0.01f;
static float buttonYMargin = 0.01f;
    
unsigned int createButton(const String& text, ButtonBox* box)
{
    box->buttons.push_back({});
    Button* button = &box->buttons.back();
    button->id = box->nextID++;
    button->text = text;
    button->hot = false;
    button->active = false;
    box->currentHeaderColor = &headerIdleColor;
    unsigned int numButtons = box->buttons.size;
    box->dimensions.y += buttonH + buttonYMargin;    
    return button->id;
}

Vec2 getButtonScreenPos(unsigned int buttonID, ButtonBox* box)
{
    for (unsigned int i = 0; i < box->buttons.size; ++i)
    {
        if (box->buttons[i].id == buttonID)
        {
            return
                box->screenPos +
                Vec2(buttonXMargin,
                     -headerH + ( -buttonYMargin - i * buttonH - i * buttonYMargin));
        }
    }
    warn("Tried to find position for button that does not exist - ID: %d\n", buttonID);
    return Vec2();
}

bool isCursorOnHeader(ButtonBox* box)
{
    //Vec2 pos = getButtonScreenPos(button->id, box);

    bool result = vec2WithinRect(g_mousePos, box->screenPos + Vec2(box->dimensions.x, -headerH),
                                 box->screenPos);
    return result;        
}

bool isCursorOnBox(ButtonBox* box)
{
    bool result = vec2WithinRect(g_mousePos, box->screenPos + Vec2(box->dimensions.x, -box->dimensions.y),
                                 box->screenPos);
    return result;        

}


static bool isCursorOnElement(Button* button, ButtonBox* box)
{
    Vec2 pos = getButtonScreenPos(button->id, box);

    bool result = vec2WithinRect(g_mousePos, pos + Vec2(backgroundW - buttonXMargin, -buttonH), pos);
    return result;        
}

bool doButton(unsigned int buttonID, ButtonBox* box)
{
    Button* button = NULL;
    for (unsigned int i = 0; i < box->buttons.size; ++i)
    {
        if (box->buttons[i].id == buttonID)
        {
            button = &box->buttons[i];
            break;
        }
    }
    if (!button)
    {
        warn("Tried to poll button that does not exist - ID: %d\n", buttonID);
        return false;
    }
    if (button->active)
    {
        if (isCursorOnElement(button, box))
        {
            if (g_input.mouseLeftReleased)
            {        
                button->active = false;
                return true;
            }
            return false;
        }
        button->active = false;
        return false;
    }
    return false;
}

static void updateButton(Button* button, ButtonBox* box)
{
    if (g_mouseState)
    {        
        if (isCursorOnElement(button, box))
        {            
            button->hot = true;            
            if (g_input.mouseLeftClicked)
            {
                button->active = true;
            }
        }
        else
        {
            if (!button->active)
            {
                button->hot = false;
            }
        }
    }
}

bool initButtonBox(const String& title, unsigned int id, ButtonBox* box)
{
    box->id = id;
    box->active = true;
    box->buttons.init(10);
    box->title = title;
    box->screenPos = Vec2(-0.3f, 0.3f);
    box->nextID = 0;
    box->drag = false;
    box->currentHeaderColor = &headerIdleColor;
    box->inFocus = false;
    box->dimensions = Vec2(backgroundW, headerH + buttonYMargin);
    return true;
}

void setButtonBoxActive(bool val, ButtonBox* box)
{
    box->active = val;
}

void updateButtonBox(ButtonBox* box)
{
    if (box->active)
    {
        if (g_mouseState)
        {
         
            if (isCursorOnHeader(box))
            {
                box->currentHeaderColor = &headerHotColor;
                if (g_input.mouseLeftClicked)
                {
                    box->drag = true;
                    box->dragPoint = g_mousePos - box->screenPos;
                }
            }
            else if (!box->drag)
            {
                box->currentHeaderColor = &headerIdleColor;
            }

            box->inFocus = vec2WithinRect(g_mousePos,
                                          box->screenPos + Vec2(box->dimensions.x, -box->dimensions.y),
                                          box->screenPos);
        
            if (box->drag && g_input.mouseLeftReleased)
            {
                box->drag = false;
            }

            if (box->drag)
            {
                box->screenPos = g_mousePos - box->dragPoint;
            }
            
        }                
                         
        for (unsigned int i = 0; i < box->buttons.size; ++i)
        {
            Button* button = &box->buttons[i];
            updateButton(button, box);
        } 
    }            
}
    
void drawButtonBox(ButtonBox* box)
{
    if (box->active)
    {
        g_uiRenderer->setView(g_view);

        static Quad buttonQuad(0, 0,
                               0, 0 - buttonH,
                               backgroundW - buttonXMargin * 2, -buttonH,
                               backgroundW - buttonXMargin * 2, 0);
    
        static Quad headerQuad(0, 0,
                               0, -headerH,
                               backgroundW, -headerH,
                               backgroundW, 0);

        float currentRelativePos = 0;
        float backgroundH = box->dimensions.y - headerH;
        unsigned int numButtons = box->buttons.size;
        Quad background(0, 0,
                        0, -backgroundH,
                        box->dimensions.x, -backgroundH,
                        box->dimensions.x, 0);
        g_uiRenderer->submitQuad(headerQuad,
                                 box->screenPos + Vec2(0, currentRelativePos),
                                 *box->currentHeaderColor);
        float titleWidth = calculateStringWidth(box->title, g_arialFont);
        float titlePos = (backgroundW * 0.5f) - (titleWidth * 0.5f);
        g_uiRenderer->submitText(box->title,
                                 box->screenPos + Vec2(titlePos, - headerH + headerH * 0.3f)
                                 , 0.2f);
        currentRelativePos += (-headerH);
        g_uiRenderer->submitQuad(background,
                                 box->screenPos + Vec2(0, currentRelativePos),
                                 backgroundColor);

        for (unsigned int i = 0; i < numButtons; ++i)
        {
            Button* button = &box->buttons[i];
            Vec4* buttonColor;
            if (button->active)
            {
                buttonColor = &buttonActiveColor;
            }
            else if (button->hot)
            {
                buttonColor = &buttonHotColor;
            }
            else 
            {
                buttonColor = &buttonIdleColor;
            }
            Vec2 pos = getButtonScreenPos(button->id, box);
            g_uiRenderer->submitQuad(buttonQuad, pos, *buttonColor);
            if (button->text.size > 0)
            {
                g_uiRenderer->submitText(button->text, pos + Vec2(.01f, -buttonH + buttonH * 0.2f), 0.2f);
            }
        }
    }
}

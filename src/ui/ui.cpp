#include "ui.h"
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
    
unsigned int createButton(const std::string& text, UI* ui)
{
    ui->buttons.push_back({});
    Button* button = &ui->buttons.back();
    button->id = ui->nextID++;
    button->text = std::string(text);
    button->hot = false;
    button->active = false;
    unsigned int numButtons = ui->buttons.size;
    ui->dimensions.y += buttonH + buttonYMargin * 2;

    
    return button->id;
}

Vec2 getButtonScreenPos(unsigned int buttonID, UI* ui)
{
    for (unsigned int i = 0; i < ui->buttons.size; ++i)
    {
        if (ui->buttons[i].id == buttonID)
        {
            return
                ui->screenPos +
                Vec2(buttonXMargin,
                     -headerH + ( -buttonYMargin - i * buttonH - i * buttonYMargin));
        }
    }
    warn("Tried to find position for button that does not exist - ID: %d\n", buttonID);
    return Vec2();
}

static bool cursorOnElement(Button* button, UI* ui)
{
    Vec2 pos = getButtonScreenPos(button->id, ui);

    bool result = vec2WithinRect(g_mousePos, pos + Vec2(backgroundW - buttonXMargin, -buttonH), pos);
    return result;        
}

bool doButton(unsigned int buttonID, UI* ui)
{
    Button* button = NULL;
    for (unsigned int i = 0; i < ui->buttons.size; ++i)
    {
        if (ui->buttons[i].id == buttonID)
        {
            button = &ui->buttons[i];
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
        if (cursorOnElement(button, ui))
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

static void updateButton(Button* button, UI* ui)
{
    if (g_mouseState)
    {        
        if (cursorOnElement(button, ui))
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

bool initUI(const std::string& title, UI* ui)
{
    ui->buttons.init(10);
    ui->title = std::string(title);
    ui->screenPos = Vec2(-0.3f, 0.3f);
    ui->nextID = 0;
    ui->drag = false;
    ui->currentHeaderColor = &headerIdleColor;
    ui->inFocus = false;
    ui->dimensions = Vec2(backgroundW, headerH);
    return true;
}

void updateUI(UI* ui)
{
   
    if (g_mouseState)
    {
         
        if (vec2WithinRect(g_mousePos, ui->screenPos + Vec2(ui->dimensions.x, -headerH),
                           ui->screenPos))
        {
            ui->currentHeaderColor = &headerHotColor;
            if (g_input.mouseLeftClicked)
            {
                ui->drag = true;
                ui->dragPoint = g_mousePos - ui->screenPos;
            }
        }
        else if (!ui->drag)
        {
            ui->currentHeaderColor = &headerIdleColor;
        }

        ui->inFocus = vec2WithinRect(g_mousePos,
                                     ui->screenPos + Vec2(ui->dimensions.x, -ui->dimensions.y),
                                     ui->screenPos);
        
        if (ui->drag && g_input.mouseLeftReleased)
        {
            ui->drag = false;
        }

        if (ui->drag)
        {
            ui->screenPos = g_mousePos - ui->dragPoint;
        }
            
    }                
            
             
    for (unsigned int i = 0; i < ui->buttons.size; ++i)
    {
        Button* button = &ui->buttons[i];
        updateButton(button, ui);
    } 
            
}
    
void drawUI(UI* ui)
{
    g_renderer->setView(g_view);

    static Quad buttonQuad(0, 0,
                           0, 0 - buttonH,
                           backgroundW - buttonXMargin * 2, -buttonH,
                           backgroundW - buttonXMargin * 2, 0);
    
    static Quad headerQuad(0, 0,
                           0, -headerH,
                           backgroundW, -headerH,
                           backgroundW, 0);

    float currentRelativePos = 0;
    float backgroundH = ui->dimensions.y - headerH;
    unsigned int numButtons = ui->buttons.size;
    Quad background(0, 0,
                    0, -backgroundH,
                    ui->dimensions.x, -backgroundH,
                    ui->dimensions.x, 0);
    g_uiRenderer->submitQuad(headerQuad,
                           ui->screenPos + Vec2(0, currentRelativePos),
                           *ui->currentHeaderColor);
    float titleWidth = calculateStringWidth(ui->title, g_arialFont);
    float titlePos = (backgroundW * 0.5f) - (titleWidth * 0.5f);
    g_uiRenderer->submitText(ui->title,
                           ui->screenPos + Vec2(titlePos, - headerH + headerH * 0.3f)
                           , 0.2f);
    currentRelativePos += (-headerH);
    g_uiRenderer->submitQuad(background,
                           ui->screenPos + Vec2(0, currentRelativePos),
                           backgroundColor);

    for (unsigned int i = 0; i < numButtons; ++i)
    {
        Button* button = &ui->buttons[i];
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
        Vec2 pos = getButtonScreenPos(button->id, ui);
        g_uiRenderer->submitQuad(buttonQuad, pos, *buttonColor);
        if (button->text.size() > 0)
        {
            g_uiRenderer->submitText(button->text, pos + Vec2(.01f, -buttonH + buttonH * 0.2f), 0.2f);
        }
    }
}

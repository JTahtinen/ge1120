#pragma once
#include "game/game.h"
#include "game/editor.h"
#include "ui/ui.h"
#include "util/timer.h"
#include <SDL2/SDL.h>

struct Application
{
    int windowWidth;
    int windowHeight;
    SDL_Window *win;
    SDL_GLContext glContext;
    Game* game;
    Editor editor;
    UI debugUI;
    bool running;
    bool viewDebugUI;
    bool displayMemoryInfo;
    bool displayTimerInfo;
    bool displayWorldInfo;
    bool editorMode;
    unsigned int frameCap;
    Timer frameTimer;
};

bool init(Application* app);
void run(Application* app);
void destroy(Application* app);

#pragma once
#include <SDL2/SDL.h>

#define MAX_EVENTS 50
struct EventContainer
{
    SDL_Event inputEvents[MAX_EVENTS];
    SDL_Event windowEvents[MAX_EVENTS];
    int numInputEvents;
    int numWindowEvents;

    void update();
};

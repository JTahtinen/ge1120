#include "event.h"

void EventContainer::update()
{
    this->numInputEvents = 0;
    this->numWindowEvents = 0;
    SDL_Event ev;

    while (SDL_PollEvent(&ev))
    {
        switch (ev.type)
        {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEMOTION:
        {
            if (this->numInputEvents < MAX_EVENTS)
            {
                this->inputEvents[this->numInputEvents++] = ev;
            }
            break;
        }
        case SDL_WINDOWEVENT:
        {
            if (this->numWindowEvents < MAX_EVENTS)
            {
                this->windowEvents[this->numWindowEvents++] = ev;
            }
        }
        }
    }
}
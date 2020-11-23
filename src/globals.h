#pragma once
#include "system/event.h"
#include "system/input.h"
#include "graphics/renderer.h"

class Texture;

extern unsigned int entityVAO;
extern unsigned int thingyVAO;
extern Renderer g_renderer;
extern Texture* entityTexture;
extern Texture* thingyTexture;
extern EventContainer g_events; 
extern Input g_input;
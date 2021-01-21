#pragma once
#include "system/event.h"
#include "system/input.h"
#include "graphics/renderer.h"
#include "graphics/vertexarray.h"
class Texture;

extern VertexArray* entityVAO;
extern VertexArray* thingyVAO;
extern Renderer g_renderer;
extern Texture* entityTexture;
extern Texture* thingyTexture;
extern EventContainer g_events; 
extern Input g_input;
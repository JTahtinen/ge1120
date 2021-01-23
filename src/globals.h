#pragma once
#include "system/event.h"
#include "system/input.h"
#include "graphics/renderer.h"
#include "graphics/vertexarray.h"
class Texture;

extern VertexArray* entityVAO;
extern VertexArray* thingyVAO;
extern Renderer* g_renderer;
extern Texture* entityTexture;
extern Texture* thingyTexture;
extern EventContainer g_events; 
extern Input g_input;

extern unsigned int g_boundShaderID;
extern unsigned int g_boundVAOID;
extern unsigned int g_boundVBOID;

extern unsigned int g_squareFillIBO;
extern unsigned int g_squareLineIBO;

extern bool g_enableWireframe;
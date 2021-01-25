#pragma once
#include "system/event.h"
#include "system/input.h"
#include "graphics/renderer.h"

class Texture;
struct Renderer;
struct VertexArray;


extern VertexArray* g_entityVAO;
extern VertexArray* g_thingyVAO;
extern Renderer* g_renderer;
extern Texture* g_entityTexture;
extern Texture* g_thingyTexture;
extern IndexBuffer* g_squareFillIBO;
extern IndexBuffer* g_squareLineIBO;

extern EventContainer g_events; 
extern Input g_input;

extern unsigned int g_boundShaderID;
extern unsigned int g_boundVAOID;
extern unsigned int g_boundVBOID;
extern unsigned int g_boundIBOID;


extern bool g_enableWireframe;

void initGlobals();
void deleteGlobals();
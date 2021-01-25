#pragma once
#include "system/event.h"
#include "system/input.h"

class Texture;
struct Renderer;
struct VertexArray;
class Shader;
struct IndexBuffer;

extern VertexArray* g_entityVAO;
extern VertexArray* g_thingyVAO;
extern Renderer* g_renderer;
extern Texture* g_entityTexture;
extern Texture* g_thingyTexture;
extern IndexBuffer* g_squareFillIBO;
extern IndexBuffer* g_squareLineIBO;

extern Shader*      g_basicShader;
extern Shader*      g_lineShader;
extern Shader*      g_quadShader;

extern EventContainer g_events; 
extern Input g_input;

extern unsigned int g_boundShaderID;
extern unsigned int g_boundVAOID;
extern unsigned int g_boundVBOID;
extern unsigned int g_boundIBOID;

extern float g_aspect;

extern bool g_enableWireframe;

bool initGlobals();
void deleteGlobals();
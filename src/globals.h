#pragma once
#include "system/event.h"
#include "system/input.h"
#include "system/memory.h"

struct Texture;
struct Renderer;
struct VertexArray;
class Shader;
struct IndexBuffer;
struct Font;
struct Tile;

extern Memory g_memory;

extern bool g_mouseState;
extern bool g_debugMode;

extern VertexArray* g_entityVAO;
extern VertexArray* g_thingyVAO;
extern Renderer* g_renderer;
extern Texture* g_entityTexture;
extern Texture* g_thingyTexture;
extern Texture* g_redTex;
extern Texture* g_greenTex;
extern Texture* g_wallTex;

extern IndexBuffer* g_squareFillIBO;
extern IndexBuffer* g_squareLineIBO;

extern Tile* g_voidTile;
extern Tile* g_thingyTile;
extern Tile* g_wallTile;

extern Shader*      g_basicShader;
extern Shader*      g_lineShader;
extern Shader*      g_quadShader;
extern Shader*      g_letterShader;

extern EventContainer g_events; 
extern Input g_input;

extern unsigned int g_boundShaderID;
extern unsigned int g_boundVAOID;
extern unsigned int g_boundVBOID;
extern unsigned int g_boundIBOID;

extern Font*        g_arialFont;

extern float g_aspect;

extern float g_frameTime;

extern bool g_enableWireframe;

extern bool initGlobals();
extern void deleteGlobals();

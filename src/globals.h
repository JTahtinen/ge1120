#pragma once
#include "system/event.h"
#include "system/input.h"
#include "system/memory.h"
#include "math/vec2.h"
#include "math/mat3.h"

#define STRING_BUFFER_SIZE ((CHAR_BIT * sizeof(int) - 1) / 3 + 2)

struct Texture;
struct Renderer;
struct VertexArray;
class Shader;
struct IndexBuffer;
struct Font;
struct Tile;
struct Sprite;

extern Memory g_memory;

extern bool g_mouseState;
extern bool g_debugMode;

extern VertexArray* g_entityVAO;
extern VertexArray* g_thingyVAO;
extern Renderer* g_renderer;
extern Renderer* g_uiRenderer;
extern Texture* g_entityTexture;
//extern Texture* g_thingyTexture;
extern Texture* g_redTex;
extern Texture* g_greenTex;
//extern Texture* g_wallTex;

extern IndexBuffer* g_squareFillIBO;
extern IndexBuffer* g_squareLineIBO;

extern Tile* g_voidTile;
//extern Tile* g_thingyTile;
//extern Tile* g_wallTile;

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

extern char* stringBuffer;

extern Font* g_arialFont;

extern float g_aspect;

extern float g_frameTime;

extern bool g_enableWireframe;

extern Vec2 g_mousePosRaw;
extern Vec2 g_mousePos;
extern Vec2 g_projectedMousePos;

extern Mat3 g_view;

extern unsigned int g_screenContext;

extern bool initGlobals();
extern void deleteGlobals();

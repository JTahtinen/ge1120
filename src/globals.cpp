#include "globals.h"
#include "graphics/texture.h"
VertexArray* g_entityVAO;
VertexArray* g_thingyVAO;
Renderer* g_renderer;
Texture* g_entityTexture;
Texture* g_thingyTexture;
IndexBuffer* g_squareFillIBO;
IndexBuffer* g_squareLineIBO;
EventContainer g_events; 
Input g_input;
unsigned int g_boundShaderID;
unsigned int g_boundVAOID;
unsigned int g_boundVBOID;
unsigned int g_boundIBOID;
bool g_enableWireframe;


#define INIT(name, type, args) (name) = (new type(args))

#define INITARRAY(name, type, amt) (name) = new type[amt]

#define DEL(name) (delete name); (name = nullptr)


void initGlobals()
{
    INIT(g_entityVAO, VertexArray, );
    INIT(g_thingyVAO, VertexArray, );
    INIT(g_renderer, Renderer, );
    g_thingyTexture = Texture::loadTexture("res/textures/testImage.bmp");
    g_entityTexture = Texture::loadTexture("res/textures/dude.bmp");

    g_boundShaderID = 0;
    g_boundVAOID = 0;
    g_boundVBOID = 0;
    g_boundIBOID = 0;
    INIT(g_squareFillIBO, IndexBuffer, );
    INIT(g_squareLineIBO, IndexBuffer, );
    
    g_enableWireframe = false;
}

void deleteGlobals()
{
    DEL(g_entityVAO);
    DEL(g_thingyVAO);
    DEL(g_renderer);
    DEL(g_entityTexture);
    DEL(g_thingyTexture);
    DEL(g_squareFillIBO);
    DEL(g_squareLineIBO);
    g_boundShaderID = 0;
    g_boundVAOID = 0;
    g_boundVBOID = 0;
    g_boundIBOID = 0;
}
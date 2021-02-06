#include "globals.h"
#include "graphics/texture.h"
#include "graphics/shader.h"
#include "graphics/vertexarray.h"
#include "graphics/renderer.h"

Memory g_memory;

VertexArray *g_entityVAO;
VertexArray *g_thingyVAO;
Renderer *g_renderer;
Texture *g_entityTexture;
Texture *g_thingyTexture;
IndexBuffer *g_squareFillIBO;
IndexBuffer *g_squareLineIBO;
EventContainer g_events;
Input g_input;
unsigned int g_boundShaderID;
unsigned int g_boundVAOID;
unsigned int g_boundVBOID;
unsigned int g_boundIBOID;
bool g_enableWireframe;

Shader*      g_basicShader;
Shader*      g_lineShader;
Shader*      g_quadShader;

float g_aspect;

#define INIT(name, type, args) (name) = (new type(args))

#define INITARRAY(name, type, amt) (name) = new type[amt]

#define DEL(name)  \
    (delete name); \
    (name = nullptr)

static void initBuffers()
{
    float entityVertices[]{
        -0.1f, -0.1f, 0.0f, 1.0f,
        0.1f, -0.1f, 1.0f, 1.0f,
        0.1f, 0.1f, 1.0f, 0.0f,
        -0.1f, 0.1f, 0.0f, 0.0f};

    g_entityVAO->bind();
    Buffer *entityVBO = new Buffer();
    entityVBO->setData(entityVertices, sizeof(entityVertices));
    BufferLayout entityLayout;
    entityLayout.addLayoutElement(GL_FLOAT, 2);
    entityLayout.addLayoutElement(GL_FLOAT, 2);
    entityVBO->setLayout(&entityLayout);
    g_entityVAO->addBuffer(entityVBO);

    float thingyVertices[]{
        -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.0f, 0.0f};

    g_thingyVAO->bind();
    Buffer *thingyVBO = new Buffer();
    thingyVBO->setData(thingyVertices, sizeof(thingyVertices));
    BufferLayout thingyLayout;
    thingyLayout.addLayoutElement(GL_FLOAT, 2);
    thingyLayout.addLayoutElement(GL_FLOAT, 2);

    unsigned int fillIndices[]{
        0, 1, 2, 2, 3, 0};

    g_squareFillIBO->setData(fillIndices, 6);

    unsigned int lineIndices[]{
        0, 1, 1, 2, 2, 0, 0, 3, 3, 2};

    g_squareLineIBO->setData(lineIndices, 10);
}

bool initGlobals()
{
    g_memory.init(MB(1));
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

    g_basicShader = Shader::load("res/shaders/basic.vs", "res/shaders/basic.fs");
    if (!g_basicShader->bind())
    {
        return false;
    }
    g_basicShader->setUniform1f("u_Aspect", g_aspect);
    g_basicShader->setUniform1i("u_Texture", 0);
    g_lineShader = Shader::load("res/shaders/line.vs", "res/shaders/line.fs");
    if (!g_lineShader->bind())
    {
        return false;
    }
    g_lineShader->setUniform4f("u_Color", 1.0f, 0, 0, 1.0f);
    g_lineShader->setUniform1f("u_Aspect", g_aspect);

    g_quadShader = Shader::load("res/shaders/colorquad.vs", "res/shaders/colorquad.fs");
    if (!g_quadShader->bind())
    {
        return false;
    }
    g_quadShader->setUniform1f("u_Aspect", g_aspect);

    g_renderer->shader = g_basicShader;
    g_renderer->lineShader = g_lineShader;
    g_renderer->quadShader = g_quadShader;

    initBuffers();
    return true;
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
    DEL(g_basicShader);
    DEL(g_lineShader);
    DEL(g_quadShader);
    g_boundShaderID = 0;
    g_boundVAOID = 0;
    g_boundVBOID = 0;
    g_boundIBOID = 0;
}
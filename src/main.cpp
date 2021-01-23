#include <iostream>
#include <cstdint>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include "globals.h"
#include "defs.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include <SDL2/SDL_image.h>
#include "math/vec2.h"
#include "game/game.h"
#include "graphics/vertexarray.h"

static bool running;

EventContainer g_events;
Input g_input;

Renderer* g_renderer;

unsigned int g_boundShaderID;
unsigned int g_boundVAOID;
unsigned int g_boundVBOID;
unsigned int g_boundIBOID;

static int windowWidth;
static int windowHeight;

static SDL_Window *win;

static Shader *shader;
static Shader *lineShader;
static Shader* quadShader;

static Game *game;

VertexArray* g_entityVAO;
VertexArray* g_thingyVAO;

Texture* g_entityTexture;
Texture* g_thingyTexture;


IndexBuffer* g_squareFillIBO;
IndexBuffer* g_squareLineIBO;

bool g_enableWireframe;

static VertexArray* lineVAO;

static void updateInputs()
{
    g_input.update();
    if (g_input.isKeyPressed(KEY_ESCAPE))
    {
        running = false;
    }
    if (g_input.isKeyTyped(KEY_Q))
    {
        g_enableWireframe = !g_enableWireframe;
    }
    
}


static void updateWindow()
{

    for (int i = 0; i < g_events.numWindowEvents; ++i)
    {
        SDL_Event &ev = g_events.windowEvents[i];
        switch (ev.window.event)
        {
        case SDL_WINDOWEVENT_CLOSE:
            running = false;
            break;
        }
    }
   // GLCALL(glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_INT, 0));
    g_renderer->flush();
    SDL_GL_SwapWindow(win);
}

static void updateGame()
{
    
    static float red = 0.1f;
    static float green = 0.12f;
    static float blue = 0.15;
    static float colorDir = 1.0f;
   /* if (colorOffset >= 0.4f)
        colorDir = -1.0f;
    else if (colorOffset <= 0.1f)
        colorDir = 1.0f;

    colorOffset += colorDir * 0.0005f;
    */
   // drawEntity(thingyVao, thingyTexture, Vec2(0, 0));
    //drawEntity(player.vao, player.texture, player.pos);
    shader->setUniform4f("u_Color", red, green, blue, 1.0f);
    //shader->setUniform2f("u_Offset", player.pos.x, player.pos.y);
    game->update();
    game->render();
    static Mat3 ident = Mat3::identity();
    lineShader->setUniformMat3("u_Model", ident);
    lineShader->setUniformMat3("u_View", ident);
    //g_renderer.renderLine(lineVAO);
    g_renderer->submitLine(-0.25f, -0.25f, -0.15f, -0.3f);
    g_renderer->submitLine(0, 0, 0.5f, 0.05f);
    g_renderer->submitQuad(-0.2f, -0.3f, -0.2f, -0.1f, 0.2f, -0.1f, 0.2f, -0.3f);
}

static void updateSystem()
{
    g_events.update();
}

static void run()
{
    running = true;
    
    while (running)
    {
        GLCALL(glClear(GL_DEPTH_BUFFER_BIT));
        GLCALL(glClear(GL_COLOR_BUFFER_BIT));
        updateSystem();
        updateInputs();
        updateGame();
        updateWindow();
    }
}

SDL_GLContext glContext;

static void initEntity()
{
    float vertices[]{
            -0.1f, -0.1f, 0.0f, 1.0f,
            0.1f, -0.1f, 1.0f, 1.0f,
            0.1f, 0.1f, 1.0f, 0.0f,
            -0.1f, 0.1f, 0.0f, 0.0f};

        g_entityVAO = new VertexArray();
        Buffer* entityVBO = new Buffer();
        entityVBO->setData(vertices, sizeof(vertices));
        BufferLayout layout;
        layout.addLayoutElement(GL_FLOAT, 2);
        layout.addLayoutElement(GL_FLOAT, 2);
        entityVBO->setLayout(&layout);
        g_entityVAO->addBuffer(entityVBO);
        std::cout << entityVBO->id << std::endl;
       
        g_entityTexture = Texture::loadTexture("res/textures/dude.bmp");
        g_entityTexture->bind();
}



static bool start()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
    {
        std::cout << "SDL init successful!" << std::endl;
        windowWidth = 1280;
        windowHeight = 720;
        float aspect = (float)windowWidth / (float)windowHeight;

        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_ShowCursor(true);
        SDL_SetRelativeMouseMode(SDL_TRUE);
#ifdef DEBUG
        const char *title = "Ge1120 (DEBUG)";
#else
        const char *title = "Ge1120 (RELEASE)";
#endif
        win = SDL_CreateWindow(title,
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               windowWidth, windowHeight, SDL_WINDOW_OPENGL);

        if (!win)
        {
            std::cout << "[ERROR] Could not create window!" << std::endl;
            running = false;
            return false;
        }
        glContext = SDL_GL_CreateContext(win);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        GLenum status = glewInit();
        glClearColor(0.3f, 0.4f, 0.1f, 1.0f);

        glLineWidth(2);
        glEnable(GL_LINE_SMOOTH);

        float linePoints[]
        {
            -0.2f, -0.3f,
            0.3f, 0.2f
        };

        lineVAO = new VertexArray();
        Buffer* lineBuffer = new Buffer();
        std::cout << lineBuffer->id << std::endl;
        lineBuffer->setData(linePoints, sizeof(linePoints));
        BufferLayout lineLayout;
        lineLayout.addLayoutElement(GL_FLOAT, 2);
        lineBuffer->setLayout(&lineLayout);
        lineVAO->addBuffer(lineBuffer);


        float vertices[]{
            -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.0f, 0.0f};

        g_thingyVAO = new VertexArray();
        Buffer* thingyVBO = new Buffer();
        std::cout << thingyVBO->id << std::endl;
        thingyVBO->setData(vertices, sizeof(vertices));
        BufferLayout layout;
        layout.addLayoutElement(GL_FLOAT, 2);
        layout.addLayoutElement(GL_FLOAT, 2);
     

        g_thingyTexture = Texture::loadTexture("res/textures/testImage.bmp");
        g_thingyTexture->bind();

        unsigned int fillIndices[]{
            0, 1, 2, 2, 3, 0};


        g_squareFillIBO = new IndexBuffer();
        g_squareFillIBO->setData(fillIndices, 6);

        unsigned int lineIndices[]
        {
            0, 1, 1, 2, 2, 0, 0, 3, 3, 2
        };
        
        g_squareLineIBO = new IndexBuffer();
        g_squareLineIBO->setData(lineIndices, 10);

        shader = Shader::load("res/shaders/basic.vs", "res/shaders/basic.fs");
        if (!shader->bind())
        {
            return false;
        }
        shader->setUniform1f("u_Aspect", aspect);
        shader->setUniform1i("u_Texture", 0);
        lineShader = Shader::load("res/shaders/line.vs", "res/shaders/line.fs");
        if (!lineShader->bind())
        {
            return false;
        }
        lineShader->setUniform4f("u_Color", 1.0f, 0, 0, 1.0f);
        lineShader->setUniform1f("u_Aspect", aspect);
        
        quadShader = Shader::load("res/shaders/colorquad.vs", "res/shaders/colorquad.fs");
        if (!quadShader->bind())
        {
            return false;
        }
        quadShader->setUniform1f("u_Aspect", aspect);

        g_renderer = new Renderer();
        g_renderer->shader = shader;
        g_renderer->lineShader = lineShader;
        g_renderer->quadShader = quadShader;

		initEntity();
        game = new Game();
        run();
        delete game;
        delete g_thingyTexture;
        delete g_entityTexture;
        //GLCALL(glDeleteVertexArrays(1, &vao));
    }
    else
    {
        std::cout << "SDL init failed!" << std::endl;
        return false;
    }
    return true;
}
int main()
{
#ifdef DEBUG
    std::cout << "GE1120 DEBUG build" << std::endl;
#else
    std::cout << "GE1120 RELEASE build" << std::endl;
#endif
    if (!start())
    {
        std::cout << "[ERROR] Game initialization failed" << std::endl;
    }
    else
    {
        delete g_renderer;
        delete shader;
        delete lineShader;
        delete quadShader;
        delete g_entityVAO;
        delete g_thingyVAO;
        delete lineVAO;
        delete g_squareFillIBO;
        delete g_squareLineIBO;
        g_renderer = nullptr;
        shader = nullptr;
        lineShader = nullptr;
        quadShader = nullptr;
        g_entityVAO = nullptr;
        g_thingyVAO = nullptr;
        lineVAO = nullptr;
        g_squareFillIBO = nullptr;
        g_squareLineIBO = nullptr;
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(win);
    }
    std::cout << "Exiting program." << std::endl;
    IMG_Quit();
    SDL_Quit();
    return 0;
}
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

static bool running;

EventContainer g_events;
Input g_input;

Renderer g_renderer;

static int windowWidth;
static int windowHeight;

static SDL_Window *win;
static Shader *shader;

static Game *game;

unsigned int entityVAO;
unsigned int thingyVAO;
Texture* entityTexture;
Texture* thingyTexture;

static void updateInputs()
{
    g_input.update();
    if (g_input.isKeyPressed(KEY_ESCAPE))
    {
        running = false;
    }
    
}

static unsigned int numIndices;

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
    SDL_GL_SwapWindow(win);
}

static void updateGame()
{
    
    static float colorOffset = 0.3f;
    static float colorDir = 1.0f;
    if (colorOffset >= 0.4f)
        colorDir = -1.0f;
    else if (colorOffset <= 0.1f)
        colorDir = 1.0f;

    colorOffset += colorDir * 0.0005f;
   // drawEntity(thingyVao, thingyTexture, Vec2(0, 0));
    //drawEntity(player.vao, player.texture, player.pos);
    shader->setUniform4f("u_Color", colorOffset, 0, 0, 1.0f);
    //shader->setUniform2f("u_Offset", player.pos.x, player.pos.y);
    game->update();
    game->render();
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

        GLuint vbo;
        GLCALL(glGenVertexArrays(1, &entityVAO));
        GLCALL(glBindVertexArray(entityVAO));
        GLCALL(glGenBuffers(1, &vbo));
        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
        GLCALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
        GLCALL(glEnableVertexAttribArray(0));
        GLCALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (const void *)0));
        GLCALL(glEnableVertexAttribArray(1));
        GLCALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (const void *)(2 * sizeof(float))));

        entityTexture = Texture::loadTexture("res/textures/dude.bmp");
        entityTexture->bind();

        unsigned int indices[]{
            0, 1, 2, 2, 3, 0};

        numIndices = sizeof(indices) / sizeof(unsigned int);

        GLuint ibo;
        GLCALL(glGenBuffers(1, &ibo));
        GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
        GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));
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

        float vertices[]{
            -0.5f, -0.5f, 0.0f, 1.0f,
            0.5f, -0.5f, 1.0f, 1.0f,
            0.5f, 0.5f, 1.0f, 0.0f,
            -0.5f, 0.5f, 0.0f, 0.0f};

        GLuint vbo;
        GLCALL(glGenVertexArrays(1, &thingyVAO));
        GLCALL(glBindVertexArray(thingyVAO));
        GLCALL(glGenBuffers(1, &vbo));
        GLCALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
        GLCALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
        GLCALL(glEnableVertexAttribArray(0));
        GLCALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (const void *)0));
        GLCALL(glEnableVertexAttribArray(1));
        GLCALL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (const void *)(2 * sizeof(float))));

        thingyTexture = Texture::loadTexture("res/textures/testImage.bmp");
        thingyTexture->bind();

        unsigned int indices[]{
            0, 1, 2, 2, 3, 0};

        numIndices = sizeof(indices) / sizeof(unsigned int);

        GLuint ibo;
        GLCALL(glGenBuffers(1, &ibo));
        GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
        GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));
        shader = Shader::load("res/shaders/basic.vs", "res/shaders/basic.fs");
        if (!shader->bind())
        {
            return false;
        }
        shader->setUniform1f("u_Aspect", aspect);
        shader->setUniform1i("u_Texture", 0);
        g_renderer.shader = shader;
        g_renderer.bind();
		initEntity();
        game = new Game();
        run();
        delete game;
        delete thingyTexture;
        delete entityTexture;
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
        delete shader;
        shader = nullptr;
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(win);
    }
    std::cout << "Exiting program." << std::endl;
    IMG_Quit();
    SDL_Quit();
    return 0;
}
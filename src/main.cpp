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
#include "graphics/renderer.h"
#include "system/memory.h"

static bool running;

static int windowWidth;
static int windowHeight;

static SDL_Window *win;

static Game *game;


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
    g_renderer->setView(Mat3::identity());
    g_memory.visualize();
    g_renderer->flush();
    SDL_GL_SwapWindow(win);
}

static void updateGame()
{
    void *p;
    static int* memoryHandles = new int[60000];
    static unsigned int numMemoryHandles = 0;
    ASSERT(numMemoryHandles <= 60000);

    if (g_input.isKeyTyped(KEY_N))
    {
        int handle = g_memory.reserve(10, &p);
        if (handle > -1)
        {
            memoryHandles[numMemoryHandles++] = handle;
        }
        g_memory.printState();
    }
    if (g_input.isKeyPressed(KEY_M))
    {
        for (int i = 0; i < 10; ++i)
        {
            if (g_input.isKeyTyped(KEY_0 + i))
            {
                if (i < numMemoryHandles)
                {
                    if (g_memory.release(memoryHandles[i]))
                    {
                        for (int j = i; j < numMemoryHandles - 1; ++j)
                        {
                            memoryHandles[j] = memoryHandles[j + 1];
                        }
                        --numMemoryHandles;
                    }
                    g_memory.printState();
                }
            }
        }
    }
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
    //shader->setUniform2f("u_Offset", player.pos.x, player.pos.y);
    game->update();
    game->render();
    static Mat3 ident = Mat3::identity();
    g_basicShader->setUniform4f("u_Color", red, green, blue, 1.0f);
    //   g_renderer->setView(Mat3::identity());
    //   g_renderer->submitLine(0, 0, 0.3f, 0.2f, Vec2(0.01f, -0.01f));
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

static bool start()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
    {
        message("SDL init successful!\n");
        windowWidth = 1280;
        windowHeight = 720;
        g_aspect = (float)windowWidth / (float)windowHeight;

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
            message("[ERROR] Could not create window!\n");
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

        initGlobals();
        game = new Game();
        run();
        delete game;
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
    message("GE1120 DEBUG build\n");
#else
    message("GE1120 RELEASE build\n");
#endif
    if (!start())
    {
        message("[ERROR] Game initialization failed\n");
    }
    else
    {
        deleteGlobals();
        SDL_GL_DeleteContext(glContext);
        SDL_DestroyWindow(win);
    }
    message("Exiting program.\n");
    IMG_Quit();
    SDL_Quit();
    return 0;
}
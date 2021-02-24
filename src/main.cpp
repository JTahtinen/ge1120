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
#include "math/math.h"
#include "graphics/vertexarray.h"
#include "graphics/renderer.h"
#include "system/memory.h"
#include "util/vector.h"
#include <chrono>
#include <string>
#include <thread>

struct Timer
{
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point currentTime;
    std::chrono::steady_clock::time_point lastTime;

    inline void start()
	{
		startTime = std::chrono::high_resolution_clock::now();
		currentTime = startTime;
		lastTime = startTime;
	}
	unsigned int getElapsedInMillis() const
	{
		return (unsigned int)std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
	}
	inline unsigned int getElapsedSinceLastUpdateInMillis() const
	{
		return (unsigned int)std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();
	}
	inline void update()
	{
		lastTime = currentTime;
		currentTime = std::chrono::high_resolution_clock::now();
	}
};


static bool running;

static int windowWidth;
static int windowHeight;

static SDL_Window *win;

static Game *game;

static Timer gameTimer;
static Timer frameTimer;

static int frameCap;

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

    
    static unsigned int frames = 0;
    if (g_input.isKeyTyped(KEY_B))
    {
        g_mouseState = !g_mouseState;
        if (!g_mouseState)
        {
            SDL_SetRelativeMouseMode(SDL_TRUE);
        }
        else
        {
            SDL_SetRelativeMouseMode(SDL_FALSE);
        }
    }

    static Vec2 buttonPos(-0.8f, -0.3f);
    static Quad buttonQuad(0.0f, 0.0f, 0.0f, 0.04f, 0.1f, 0.04f, 0.1f, 0.0f);
    static Vec4 buttonIdleColor(0.6f, 0.6f, 0.6f, 1.0f);
    static Vec4 buttonHighlightColor(0.8f, 0.8f, 0.8f, 1.0f);
    static Vec4* buttonColor = &buttonIdleColor;
    if (g_mouseState)
    {
        float mX = (float)g_input.mouseX / (float)windowWidth * 2.0f - 1.0f;
        float mY = -((float)(g_input.mouseY / (float)windowHeight * 2.0f - 1.0f));
        
        if (vec2IsBetween(Vec2(mX, mY), buttonPos, buttonPos + buttonQuad.point2))
        {
            buttonColor = &buttonHighlightColor;
        }
        else
        {
            buttonColor = &buttonIdleColor;
        }

    }

    /*
    static void **pointers = new void *[60000];
    static unsigned int numPointers = 0;

    ASSERT(numPointers <= 60000);

    if (g_input.isKeyTyped(KEY_N))
    {
        void *p = g_memory.reserve(10);
        if (p)
        {
            pointers[numPointers++] = p;
        }
        g_memory.printState();
    }
    if (g_input.isKeyPressed(KEY_M))
    {
        for (int i = 0; i < 10; ++i)
        {
            if (g_input.isKeyTyped(KEY_0 + i))
            {
                if (i < numPointers)
                {
                    if (g_memory.release(pointers[i]))
                    {
                        for (int j = i; j < numPointers - 1; ++j)
                        {
                            pointers[j] = pointers[j + 1];
                        }
                        --numPointers;
                    }
                    g_memory.printState();
                }
            }
        }
    }
*/

    

    unsigned int frameTimeInMillis = frameTimer.getElapsedSinceLastUpdateInMillis();    
    frameTimer.update();
    

    static unsigned int minFrameTime = 1000 / frameCap;
    
    if (frameTimeInMillis < minFrameTime)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(minFrameTime - frameTimeInMillis));
        g_frameTime = (float)minFrameTime * 0.001f;
    }
    else
    {
        g_frameTime = (float)frameTimeInMillis * 0.001f;
    }
    game->update();
    ++frames;
    
    static unsigned int framesPerSec = 0;
    static unsigned int timerAccumulator = 0;
    static unsigned int seconds = 0;
    timerAccumulator += frameTimeInMillis;
    gameTimer.update();
    if (timerAccumulator >= 1000)
    {
        seconds++;
        while (timerAccumulator > 1000)
        {
            timerAccumulator -= 1000;
        }
        framesPerSec = frames;
        frames = 0;
    }
    g_renderer->submitText("frametime: " + std::to_string((int)(g_frameTime * 1000.f))+ "ms", Vec2(-0.95f, 0.5f));
    g_renderer->submitText("fps: " + std::to_string(framesPerSec), Vec2(-0.95, 0.47f));
    g_renderer->submitText("Sec: " + std::to_string(seconds), Vec2(-0.95, 0.44f));
    
    game->render();


    g_renderer->setView(Mat3::identity());
    g_renderer->submitQuad(buttonQuad, buttonPos, *buttonColor);
}

static void updateSystem()
{
    g_events.update();
}

static void run()
{
    running = true;
    gameTimer.start();
    frameTimer.start();
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
            err("Could not create window!\n");
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

        IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

        frameCap = 60;

        if (!initGlobals())
        {
            message("[ERROR] Globals init failed!\n");
        }
        else
        {
            game = new Game();
            run();
            delete game;
        }
    }
    else
    {
        std::cout << "SDL init failed!" << std::endl;
        return false;
    }
    return true;
}

int main(int argc, char** argv)
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

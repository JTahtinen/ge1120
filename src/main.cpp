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
#include "util/vector.h"
#include <chrono>
#include <string>
#include <sstream>

class Timer
{
    std::chrono::steady_clock::time_point _startTime;
    std::chrono::steady_clock::time_point _currentTime;
    std::chrono::steady_clock::time_point _lastTime;
public:
    inline void start()
	{
		_startTime = std::chrono::high_resolution_clock::now();
		_currentTime = _startTime;
		_lastTime = _startTime;
	}
	unsigned int getElapsedInMillis() const
	{
		return (unsigned int)std::chrono::duration_cast<std::chrono::milliseconds>(_currentTime - _startTime).count();
	}
	inline unsigned int getElapsedSinceLastUpdateInMillis() const
	{
		return (unsigned int)std::chrono::duration_cast<std::chrono::milliseconds>(_currentTime - _lastTime).count();
	}
	inline void update()
	{
		_lastTime = _currentTime;
		_currentTime = std::chrono::high_resolution_clock::now();
	}
};


static bool running;

static int windowWidth;
static int windowHeight;

static SDL_Window *win;

static Game *game;
static Timer frameTimer;

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
    //g_renderer->submitText("Hello people", Vec2(-0.2f, 0.3f));
    g_memory.visualize();
    g_renderer->flush();
    SDL_GL_SwapWindow(win);
}

static void updateGame()
{
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
    frameTimer.update();
    unsigned int frameTimeInMillis = frameTimer.getElapsedSinceLastUpdateInMillis();
    g_frameTime = (float)frameTimeInMillis * 0.001;
    game->update();
    std::ostringstream ss;
    ss << frameTimeInMillis;
    std::string ftString = "frametime: ";
    ftString.append(ss.str());
    ftString.append("ms");
    g_renderer->submitText(ftString, Vec2(-0.95f, 0.5f));
    
    game->render();
}

static void updateSystem()
{
    g_events.update();
}

static void run()
{
    running = true;
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

        IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

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
int main()
{
#ifdef DEBUG
    message("GE1120 DEBUG build\n");
#else
    message("GE1120 RELEASE build\n");
#endif
    /*
    Vector<unsigned int> stuff(20);
    for (int i = 0; i < 20; ++i)
    {
        stuff.push_back(i);
    }
    while (1)
    {
        message("Vector Size: %d\nCapacity: %d\nContent:\n", stuff.size(), stuff.capacity());
        for (int i = 0; i < stuff.size(); ++i)
        {
            message("%d\n", stuff[i]);
        }
        char input;
        message("Input: ");
        std::cin >> input;
        message("\n");

        switch (input)
        {
        case '1':        
        {
            int index;
            int val;
            bool valid = false;
            while (!valid)
            {
                message("choose index: ");
                std::cin >> index;
                if (index > stuff.size())
                {
                    message("Invalid index!\n");
                    continue;
                }
                message("Choose value: ");
                std::cin >> val;
                stuff.insert(val, index);
                valid = true;
            }
        }
        break;
        case '2':
        {
            stuff.erase(0);
        }
            break;
        case '3':
        {
            stuff.erase(0, 3);
        }
            break;
        case '0':
            return 0;
        }
    }
    */
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

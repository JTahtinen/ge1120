#include <iostream>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include "globals.h"
#include "defs.h"
#include <SDL2/SDL_image.h>
#include "math/vec2.h"
#include "game/game.h"
#include "math/math.h"
#include "graphics/font.h"

#include "graphics/renderer.h"
#include "system/memory.h"
#include "util/vector.h"
#include "util/timer.h"
#include <string>
#include <thread>

#include "util/file.h"
#include "ui/ui.h"

static bool running;

static int windowWidth;
static int windowHeight;

static SDL_Window *win;

static Game *game;
static Timer frameTimer;

static unsigned int frameCap = 60;

static bool displayWorldInfo = false;
static UI* ui;
static bool viewDebugUI = false;
static bool displayMemoryInfo = false;
static bool displayTimerInfo = false;
static void updateInputs()
{
    g_input.update();
    g_mousePosRaw = Vec2((float)g_input.mouseX / (float)windowWidth * 2.0f - 1.0f,
                      -((float)g_input.mouseY / (float)windowHeight * 2.0f - 1.0f));
    g_mousePos = Vec2(g_mousePosRaw.x, g_mousePosRaw.y * (1.0f / g_aspect));
    if (g_input.isKeyPressed(KEY_ESCAPE))
    {
        running = false;
    }
}

static void updateWindow()
{
    GLCALL(glClear(GL_DEPTH_BUFFER_BIT));
    GLCALL(glClear(GL_COLOR_BUFFER_BIT));

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
    if (displayMemoryInfo)
    {
        g_memory.visualize();
    }
    g_renderer->submitText("F1 - Debug toolbox", Vec2(0.4f, -0.5f));
    g_renderer->submitText("RMB - Switch mouse mode", Vec2(0.4f, -0.53f));
    g_renderer->flush();
    SDL_GL_SwapWindow(win);
}


static void updateGame()
{
    static unsigned int debugToggleButton = createButton("Debug mode", ui);
    static unsigned int worldInfoButton = createButton("World info", ui);
    static unsigned int memoryInfoButton = createButton("Memory", ui);
    static unsigned int timerInfoButton = createButton("Timers", ui);
    
    if (viewDebugUI)
    {
        updateUI(ui);
    }
    if (displayWorldInfo)
    {
        if (g_mouseState)
        {
            
            g_renderer->setView(Mat3::identity());
            Mat3 cameraToScreen = screenToWorldProjection(game);
            Vec2 projectedMousePos = cameraToScreen * g_mousePosRaw;
            //Vec2 fixedMousePos = Vec2(g_mousePos.x, g_mousePos.y * (1.0f / g_aspect));
        
            g_renderer->submitText(std::to_string(projectedMousePos.x)
                                   + " " + std::to_string(projectedMousePos.y),
                                   g_mousePos + Vec2(0.01f, 0.02f));
            DataStrings dataStrings;
            getDataFromPos(game, projectedMousePos, &dataStrings);

            float offset = -0.07f;
            unsigned int numStrings = dataStrings.strings.size();
            float longestStringWidth = 0;
            for (unsigned int i = 0; i < numStrings; ++i)
            {
                float currentStringWidth = calculateStringWidth(dataStrings.strings[i], g_renderer->font);
                if (currentStringWidth > longestStringWidth)
                {
                    longestStringWidth = currentStringWidth;
                }
            }
            Quad textBoxDim {0, 0,
                    longestStringWidth, 0,
                    longestStringWidth, (float)numStrings * -0.06f,
                    0, (float)numStrings * -0.06f}; 
            g_renderer->submitQuad(textBoxDim, g_mousePosRaw + Vec2(0.01f, offset), Vec4(0, 0, 0, 0.5f));
            for (unsigned int i = 0; i < numStrings; ++i)
            {
                g_renderer->submitText(dataStrings.strings[i],
                                       g_mousePos + Vec2(0.01f, offset));
                offset -= 0.03f;
            }
        }
    }
    if (g_input.isKeyTyped(KEY_B) || g_input.mouseRightClicked)
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
    if (g_input.isKeyTyped(KEY_F1))
    {
        viewDebugUI = !viewDebugUI;
    }

    if (doButton(debugToggleButton, ui))
    {
        g_debugMode = !g_debugMode;
    }
    if (doButton(worldInfoButton, ui))
    {
        displayWorldInfo = !displayWorldInfo;
    }
    if (doButton(memoryInfoButton, ui))
    {
        displayMemoryInfo = !displayMemoryInfo;
    }
    if (doButton(timerInfoButton, ui))
    {
        displayTimerInfo = !displayTimerInfo;
    }
  

    game->update();
    game->render();
    if (viewDebugUI)
    {
        drawUI(ui);
    }
}

static void updateSystem()
{
    g_events.update();
}

static void run()
{
    running = true;
    unsigned int minFrameTime = 1000 / frameCap;    
    g_frameTime = minFrameTime;
    unsigned int timerAccumulator = 0;
    unsigned int framesPerSec = 0;
    unsigned int seconds = 0;
    unsigned int frames = 0;
    unsigned int frameTimeInMillis = 0;
    unsigned int uncappedFrametime = 0;
    while (running)
    {
        frameTimer.update();
        g_frameTime = (float)frameTimeInMillis * 0.001f;
        timerAccumulator += frameTimeInMillis;
        
        updateSystem();
        updateInputs();
        updateGame();
        updateWindow();
        if (g_debugMode)
        {
            g_renderer->submitText(std::to_string(timerAccumulator), Vec2(0, 0.5f));
        }
        ++frames;
        
        if (timerAccumulator >= 1000)
        {
            ++seconds;
            while (timerAccumulator > 1000)
            {
                timerAccumulator -= 1000;
            }
            framesPerSec = frames;
            frames = 0;
        }
       
        if (displayTimerInfo)
        {
            g_renderer->submitText("frametime: "
                                   + std::to_string((int)(g_frameTime * 1000.f))
                                   + "ms", Vec2(-0.95f, 0.5f));
       
            g_renderer->submitText("fps: "
                                   + std::to_string(framesPerSec), Vec2(-0.95, 0.47f));
            g_renderer->submitText("time(sec): "
                                   + std::to_string(seconds), Vec2(-0.95, 0.44f));
            g_renderer->submitText("uncapped frametime: "
                                   + std::to_string(uncappedFrametime)
                                   + "ms", Vec2(-0.95, 0.41f));
        }
        uncappedFrametime = frameTimer.getMillisSinceLastUpdate();
 
        
        if (uncappedFrametime < minFrameTime)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(minFrameTime - uncappedFrametime));
        }

        frameTimeInMillis = frameTimer.getMillisSinceLastUpdate();
                
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
        g_view = Mat3::scale(Vec2(1.0f, g_aspect));
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

        if (!initGlobals())
        {
            message("[ERROR] Globals init failed!\n");
        }
        else
        {
            static UI debugUI;
            
            //ui = createUI("DEBUG");
            if (initUI("DEBUG", &debugUI))
            {
                ui = &debugUI;
                game = new Game();
                frameTimer.start();

                run();
                delete game;
            }
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
        err("Game initialization failed\n");
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

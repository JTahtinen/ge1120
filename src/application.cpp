#include "application.h"
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include "globals.h"
#include "defs.h"
#include <SDL2/SDL_image.h>
#include "math/vec2.h"
#include "game/game.h"
#include "game/editor.h"
#include "math/math.h"
#include "graphics/font.h"
#include "graphics/renderer.h"
#include "system/memory.h"
#include "util/vector.h"
#include "util/string.h"
#include "util/timer.h"
#include <string>
#include <thread>
#include "util/file.h"
#include "ui/ui.h"


static void setMouseState(bool mode)
{
    g_mouseState = mode;
    if (!mode)
    {
        SDL_SetRelativeMouseMode(SDL_TRUE);
    }
    else
    {
        SDL_SetRelativeMouseMode(SDL_FALSE);
    }
}

static void updateInputs(Application* app)
{
    g_input.update();
    g_mousePosRaw = Vec2((float)g_input.mouseX / (float)app->windowWidth * 2.0f - 1.0f,
                      -((float)g_input.mouseY / (float)app->windowHeight * 2.0f - 1.0f));
    g_mousePos = Vec2(g_mousePosRaw.x, g_mousePosRaw.y * (1.0f / g_aspect));
    if (g_input.isKeyPressed(KEY_ESCAPE))
    {
        app->running = false;
    }
}

static void updateWindow(Application* app)
{
    for (int i = 0; i < g_events.numWindowEvents; ++i)
    {
        SDL_Event &ev = g_events.windowEvents[i];
        switch (ev.window.event)
        {
        case SDL_WINDOWEVENT_CLOSE:
            app->running = false;
            break;
        }
    }
    g_renderer->setView(Mat3::identity());
    if (app->displayMemoryInfo)
    {
        g_memory.visualize();
    }
    g_renderer->submitText("F1 - Debug toolbox", Vec2(0.4f, -0.47f));
    g_renderer->submitText("F2 - Editor Mode", Vec2(0.4f, -0.5f));
    g_renderer->submitText("RMB - Switch mouse mode", Vec2(0.4f, -0.53f));

    g_renderer->flush();
    g_uiRenderer->flush();


    SDL_GL_SwapWindow(app->win);
    GLCALL(glClear(GL_COLOR_BUFFER_BIT));
}


static void updateGame(Application* app)
{
    static unsigned int debugToggleButton = createButton("Debug mode", &app->debugUI);
    static unsigned int worldInfoButton = createButton("World info", &app->debugUI);
    static unsigned int memoryInfoButton = createButton("Memory", &app->debugUI);
    static unsigned int timerInfoButton = createButton("Timers", &app->debugUI);


    static Mat3 cameraToScreen;

    static Tile* selectedTile = g_thingyTile;;
    
    if (app->viewDebugUI)
    {
        updateUI(&app->debugUI);
    }
    if (app->editorMode)
    {
        updateEditor(&app->editor);
    }

    if (g_input.mouseRightClicked)
    {
        setMouseState(!g_mouseState);
        
    }
    if (g_input.isKeyTyped(KEY_F1))
    {
        app->viewDebugUI = !app->viewDebugUI;
    }
    if (g_input.isKeyTyped(KEY_F2))
    {
        app->editorMode = !app->editorMode;
        if (app->editorMode) setMouseState(true);
    }

    if (doButton(debugToggleButton, &app->debugUI))
    {
        g_debugMode = !g_debugMode;
    }
    if (doButton(worldInfoButton, &app->debugUI))
    {
        app->displayWorldInfo = !app->displayWorldInfo;
    }
    if (doButton(memoryInfoButton, &app->debugUI))
    {
        app->displayMemoryInfo = !app->displayMemoryInfo;
    }
    if (doButton(timerInfoButton, &app->debugUI))
    {
        app->displayTimerInfo = !app->displayTimerInfo;
    }


    app->game->update();
    app->game->render();

    
    if (g_mouseState)
    {
        g_renderer->setView(Mat3::identity());
        cameraToScreen = screenToWorldProjection(app->game);
        g_projectedMousePos = cameraToScreen * g_mousePosRaw;

        if (app->displayWorldInfo)
        {
            
            //Vec2 fixedMousePos = Vec2(g_mousePos.x, g_mousePos.y * (1.0f / g_aspect));
        
            g_renderer->submitText(toString(g_projectedMousePos.x)
                                          + " " + toString(g_projectedMousePos.y),
                                   g_mousePos + Vec2(0.01f, 0.02f));
            DataStrings dataStrings;
            getDataFromPos(app->game, g_projectedMousePos, &dataStrings);

            
            
            float offset = -0.07f;
            unsigned int numStrings = dataStrings.strings.size;
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

  

    if (app->viewDebugUI)
    {
        drawUI(&app->debugUI);
    }
    if (app->editorMode)
    {
        viewEditor(&app->editor);
    }
}

static void updateSystem()
{
    g_events.update();
}


bool init(Application* app)
{
    String s = "hello ";// + toString(123);
    String p = toString(123);
    String q = s + p;
    s.println();
    p.println();
    q.println();

    if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
    {
        app->windowWidth = 1280;
        app->windowHeight = 720;
        g_aspect = (float)app->windowWidth / (float)app->windowHeight;
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
        app->win = SDL_CreateWindow(title,
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               app->windowWidth, app->windowHeight, SDL_WINDOW_OPENGL);

        if (!app->win)
        {
            err("Could not create window!\n");
            app->running = false;
            return false;
        }
        app->glContext = SDL_GL_CreateContext(app->win);
        
        GLenum status = glewInit();
        glClearColor(0.3f, 0.4f, 0.1f, 1.0f);

        glLineWidth(2);
        glEnable(GL_LINE_SMOOTH);
        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        //glDepthRange (-1.0f, 1.0f);


        GLCALL(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

        

        if (!initGlobals())
        {
            err("Globals init failed!\n");
            return false;
        }
        else
        {
            g_renderer->createOrFindSpriteBatch(g_entityTexture);
            g_uiRenderer->createOrFindSpriteBatch(g_entityTexture);

            g_renderer->createOrFindSpriteBatch(g_thingyTexture);
            g_uiRenderer->createOrFindSpriteBatch(g_thingyTexture);

            g_renderer->createOrFindSpriteBatch(g_redTex);
            g_uiRenderer->createOrFindSpriteBatch(g_redTex);

            g_renderer->createOrFindSpriteBatch(g_greenTex);
            g_uiRenderer->createOrFindSpriteBatch(g_greenTex);

            g_renderer->createOrFindSpriteBatch(g_wallTex);
            g_uiRenderer->createOrFindSpriteBatch(g_wallTex);

            g_uiRenderer->setView(g_view);


            //message("%s\n", testString[4]);
            app->game = (Game*)g_memory.reserve(sizeof(Game));
            app->game->init();
            initUI("DEBUG", &app->debugUI);            
            
            initEditor(&app->editor, app->game);
            app->frameTimer.start();
            app->viewDebugUI = false;
            app->displayMemoryInfo = false;
            app->displayTimerInfo = false;
            app->displayWorldInfo = false;
            app->editorMode = false;
            app->frameCap = 60;
        }
    }
    else
    {
        err("SDL init failed!\n");
        return false;
    }
    return true;
}

void run(Application* app)
{
    app->running = true;
    unsigned int minFrameTime = 1000 / app->frameCap;    
    g_frameTime = minFrameTime;
    unsigned int timerAccumulator = 0;
    unsigned int framesPerSec = 0;
    unsigned int seconds = 0;
    unsigned int frames = 0;
    unsigned int frameTimeInMillis = 0;
    unsigned int uncappedFrametime = 0;
    while (app->running)
    {
        app->frameTimer.update();
        g_frameTime = (float)frameTimeInMillis * 0.001f;
        timerAccumulator += frameTimeInMillis;
        
        updateSystem();
        updateInputs(app);
        updateGame(app);
        updateWindow(app);
        if (app->displayTimerInfo)
        {
            g_renderer->submitText(toString(timerAccumulator), Vec2(0, 0.5f));
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
       
        if (app->displayTimerInfo)
        {
            g_renderer->submitText("frametime: "
                                   + toString((unsigned int)(g_frameTime * 1000.f))
                                   + "ms", Vec2(-0.95f, 0.5f));
       
            g_renderer->submitText("fps: "
                                   + toString(framesPerSec), Vec2(-0.95, 0.47f));
            g_renderer->submitText("time(sec): "
                                   + toString(seconds), Vec2(-0.95, 0.44f));
            g_renderer->submitText("uncapped frametime: "
                                   + toString(uncappedFrametime)
                                   + "ms", Vec2(-0.95, 0.41f));
        }
        uncappedFrametime = app->frameTimer.getMillisSinceLastUpdate();
 
        
        if (uncappedFrametime < minFrameTime)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(minFrameTime - uncappedFrametime));
        }

        frameTimeInMillis = app->frameTimer.getMillisSinceLastUpdate();
                
    }
}

void destroy(Application* app)
{
    deleteGlobals();
    SDL_GL_DeleteContext(app->glContext);
    SDL_DestroyWindow(app->win);
    IMG_Quit();
    SDL_Quit();
}

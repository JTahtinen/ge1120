#include "game.h"
#include "../globals.h"
#include "../defs.h"
#include "../graphics/shader.h"
#include "../graphics/texture.h"
#include "../math/mat3.h"
#include "../math/math.h"
#include "../graphics/renderer.h"

static float speed;
static float accSpeed;


Game::Game()
    : numActors(0)
{
    speed = 1.0f;
    accSpeed = speed * 2;
    player = spawnActor(Vec2(3.0f, 3.25f));
    //player->entity.rotation = 45.0f;
    camera.entity.pos = player->entity.pos;
    camera.entity.rotation = 0;
    currentCamera = &camera;
    debugCamera.entity.pos = player->entity.pos;
    for (int i = 0; i < 3; ++i)
    {
        spawnActor(Vec2(0.7f + i * 0.4f, 0));
    }
    if (tileMap.init(50, 40))
    {
        for (int y = 0; y < tileMap.height; ++y)
        {
            for (int x = 0; x < tileMap.width; ++x)
            {
                if (y % 5 == 0 && x % 8 == 0)
                {
                    tileMap.setTile(x, y, g_wallTile);
                }
                else
                {
                    tileMap.setTile(x, y, g_thingyTile);
                }
            }
        }
    }
}
Game::~Game()
{
}

Actor *Game::spawnActor(Vec2 pos)
{
    if (numActors >= MAX_GAME_ENTITIES)
    {
        std::cout << "[WARNING] Could not spawn actor - Limit reached!" << std::endl;
        return nullptr;
    }
    Actor *e = &actorPool[numActors];
    e->entity.pos = pos;
    e->entity.rotation = 0.0f;
    e->texture = g_entityTexture;
    e->vao = g_entityVAO;
    e->speed = speed;
    actors[numActors++] = e;
    return e;
}

void Game::update()
{
    player->entity.vel *= 0;
    player->entity.rotation -= g_input.mouseDeltaX * 30.0f * g_frameTime;
    Mat3 playerRotationMat = Mat3::rotation(TO_RADIANS(player->entity.rotation));
    Vec2 playerForward = playerRotationMat * Vec2(0, 1.0f);
    Vec2 playerRight(playerForward.y, -playerForward.x);
    if (g_input.isKeyPressed(KEY_A))
    {
        player->entity.vel -= playerRight * player->speed;
    }
    if (g_input.isKeyPressed(KEY_D))
    {
        player->entity.vel += playerRight * player->speed;
    }
    if (g_input.isKeyPressed(KEY_W))
    {
        player->entity.vel += playerForward * player->speed;
    }
    if (g_input.isKeyPressed(KEY_S))
    {
        player->entity.vel -= playerForward * player->speed;
    }
    if (g_input.isKeyPressed(KEY_LSHIFT))
    {
        player->speed = accSpeed;
    }
    else
    {
        player->speed = speed;
    }
    if (g_input.isKeyTyped(KEY_K))
    {
        if (currentCamera == &camera)
        {
            currentCamera = &debugCamera;
            debugCamera.entity.pos = player->entity.pos;
        }
        else
            currentCamera = &camera;
    }

    float playerFrameVel = player->speed * g_frameTime;

    if (g_input.isKeyPressed(KEY_LEFT))
    {
        debugCamera.entity.pos.x -= playerFrameVel;
    }

    if (g_input.isKeyPressed(KEY_RIGHT))
    {
        debugCamera.entity.pos.x += playerFrameVel;
    }

    if (g_input.isKeyPressed(KEY_UP))
    {
        debugCamera.entity.pos.y += playerFrameVel;
    }
    if (g_input.isKeyPressed(KEY_DOWN))
    {
        debugCamera.entity.pos.y -= playerFrameVel;
    }

    for (int i = 0; i < numActors; ++i)
    {
        Actor *e = actors[i];
        if (e->entity.vel.length() > 0)
        {
            Vec2 newVel = tileMap.checkTileCollision(e->entity.pos, e->entity.vel * g_frameTime);
            e->entity.pos += newVel;
        }
    }

    Tile* currentPlayerTile = tileMap.getTileAtPos(player->entity.pos);

    if (currentPlayerTile->barrier)
    {
        g_renderer->submitText("COLLISION!", Vec2(0, 0.3f), 1.0f);
    }

    

    //player->entity.rotation += 0.05f;
    while (player->entity.rotation > 360.0f)
    {
        player->entity.rotation -= 360.0f;
    }

    while (player->entity.rotation < 0)
    {
        player->entity.rotation += 360.0f;
    }

    camera.entity.pos = player->entity.pos + playerForward * 0.3f * g_frameTime;
    camera.entity.rotation = player->entity.rotation;
}

static Mat3 view;
void Game::render()
{

    view = Mat3::rotation(TO_RADIANS(-currentCamera->entity.rotation)) * Mat3::translation(Vec2() - currentCamera->entity.pos);
    g_renderer->setView(view);
    tileMap.draw(&camera, view);
    for (int i = 0; i < numActors; ++i)
    {
        drawActor(actors[i]);
    }
}

void Game::drawActor(Actor *e) const
{
    if (g_enableWireframe)
    {
        g_renderer->renderVAO(e->vao, e->texture, Mat3::translation(e->entity.pos) * Mat3::rotation(TO_RADIANS(e->entity.rotation)), view, RENDER_SOLID_AND_WIREFRAME);
    }
    else
    {
        g_renderer->renderVAO(e->vao, e->texture, Mat3::translation(e->entity.pos) * Mat3::rotation(TO_RADIANS(e->entity.rotation)), view, RENDER_SOLID);
    }
}


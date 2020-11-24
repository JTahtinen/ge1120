#include "game.h"
#include "../globals.h"
#include "../defs.h"
#include "../graphics/shader.h"
#include "../graphics/texture.h"
#include "../math/mat3.h"
#include "../math/math.h"


Game::Game()
{
    numActors = 0;

    player = spawnActor(Vec2(0.5f, 0.25f));
    //player->entity.rotation = 45.0f;
    camera.pos = player->entity.pos;
    camera.rotation = player->entity.rotation;
    for (int i = 0; i < 3; ++i)
    {
        spawnActor(Vec2(-0.7f + i * 0.4f, 0));
    }
    worldW = 5;
    worldH = 3;
    tileMap = new Tile[worldW * worldH];
    for (int i = 0; i < worldW * worldH; ++i)
    {
        tileMap[i].texture = thingyTexture;
    }
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
    e->texture = entityTexture;
    e->vao = entityVAO;
    e->speed = 0.001f;
    actors[numActors++] = e;
    return e;
}

void Game::update()
{

    Mat3 playerRotationMat = Mat3::rotation(TO_RADIANS(player->entity.rotation));
    Vec2 playerForward = playerRotationMat * Vec2(0, 1.0f);
    Vec2 playerRight(playerForward.y, -playerForward.x);
    player->entity.vel *= 0;

    if (g_input.isKeyPressed(KEY_LEFT))
    {
        player->entity.rotation += 0.15f;
    }
    if (g_input.isKeyPressed(KEY_RIGHT))
    {
        player->entity.rotation -= 0.15f;
    }

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

    for (int i = 0; i < numActors; ++i)
    {
        Actor *e = actors[i];
        e->entity.pos += e->entity.vel;
    }

    //player->entity.rotation += 0.05f;
    if (player->entity.rotation > 360.0f)
    {
        player->entity.rotation -= 360.0f;
    }
    
    camera.pos = player->entity.pos;
    camera.rotation = 360.0f - player->entity.rotation;
}

static Mat3 view;
void Game::render() const
{
    
    view =  Mat3::rotation(TO_RADIANS(camera.rotation)) * Mat3::translation(Vec2() - camera.pos);
        for (int y = 0; y < worldH; ++y)
    {
        for (int x = 0; x < worldW; ++x)
        {
            g_renderer.renderVAO(entityVAO, tileMap[x + y * worldW].texture, Mat3::translation(Vec2(0.2f * x, 0.2f * y)), view);
        }
    }
    for (int i = 0; i < numActors; ++i)
    {
        drawActor(actors[i]);
    }
}

void Game::drawActor(Actor *e) const
{
    g_renderer.renderVAO(e->vao, e->texture, Mat3::translation(e->entity.pos) * Mat3::rotation(TO_RADIANS(e->entity.rotation)), view);
}
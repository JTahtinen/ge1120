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

    player = spawnActor(Vec2());
    player->entity.rotation = 45.0f;
    camera.pos = player->entity.pos;
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
    player->entity.vel *= 0;
    if (g_input.isKeyPressed(KEY_A))
    {
        player->entity.vel.x = -1.0f * player->speed;
    }
    if (g_input.isKeyPressed(KEY_D))
    {
        player->entity.vel.x = 1.0f * player->speed;
    }
    if (g_input.isKeyPressed(KEY_W))
    {
        player->entity.vel.y = 1.0f * player->speed;
    }
    if (g_input.isKeyPressed(KEY_S))
    {
        player->entity.vel.y = -1.0f * player->speed;
    }

    for (int i = 0; i < numActors; ++i)
    {
        Actor *e = actors[i];
        e->entity.pos += e->entity.vel;
    }

    player->entity.rotation += 0.05f;
    if (player->entity.rotation > 360.0f)
    {
        player->entity.rotation -= 360.0f;
    }
    
    camera.pos = player->entity.pos;
}

void Game::render() const
{
    for (int y = 0; y < worldH; ++y)
    {
        for (int x = 0; x < worldW; ++x)
        {
            g_renderer.renderVAO(entityVAO, tileMap[x + y * worldW].texture, Vec2((x * 0.2f) - camera.pos.x, (y * 0.2f) - camera.pos.y), 0);
        }
    }
    for (int i = 0; i < numActors; ++i)
    {
        drawActor(actors[i]);
    }
}

void Game::drawActor(Actor *e) const
{
    g_renderer.renderVAO(e->vao, e->texture, e->entity.pos - camera.pos, e->entity.rotation);
}
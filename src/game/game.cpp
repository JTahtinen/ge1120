#include "game.h"
#include "../globals.h"
#include "../defs.h"
#include "../graphics/shader.h"
#include "../graphics/texture.h"

Game::Game()
{
    numEntities = 0;
    player = spawnEntity(Vec2());
    for (int i = 0; i < 3; ++i)
    {
        spawnEntity(Vec2(-0.7f + i * 0.4f, 0));
    }
    worldW = 5;
    worldH = 3;
    tileMap = new Tile[worldW * worldH];
    for (int i = 0; i < worldW * worldH; ++i)
    {
        tileMap[i].texture = thingyTexture;
    }
}

Entity *Game::spawnEntity(Vec2 pos)
{
    if (numEntities >= MAX_GAME_ENTITIES)
    {
        std::cout << "[WARNING] Could not spawn entity - Limit reached!" << std::endl;
        return nullptr;
    }
    Entity *e = &entityPool[numEntities];
    e->pos = pos;
    e->texture = entityTexture;
    e->vao = entityVAO;
    e->speed = 0.001f;
    entities[numEntities++] = e;
    return e;
}

void Game::update()
{
    player->vel *= 0;
    if (g_input.isKeyPressed(KEY_A))
    {
        player->vel.x = -1.0f * player->speed;
    }
    if (g_input.isKeyPressed(KEY_D))
    {
        player->vel.x = 1.0f * player->speed;
    }
    if (g_input.isKeyPressed(KEY_W))
    {
        player->vel.y = 1.0f * player->speed;
    }
    if (g_input.isKeyPressed(KEY_S))
    {
        player->vel.y = -1.0f * player->speed;
    }

    for (int i = 0; i < numEntities; ++i)
    {
        Entity *e = entities[i];
        e->pos += e->vel;
    }
}

void Game::render() const
{
    for (int y = 0; y < worldH; ++y)
    {
        for (int x = 0; x < worldW; ++x)
        {
            g_renderer.renderVAO(entityVAO, tileMap[x + y * worldW].texture, Vec2(-0.75f + x * 0.2f, y * 0.2f));
        }
    }
    for (int i = 0; i < numEntities; ++i)
    {
        drawEntity(entities[i]);
    }
}

void Game::drawEntity(Entity *e) const
{
    g_renderer.renderVAO(e->vao, e->texture, e->pos);
}
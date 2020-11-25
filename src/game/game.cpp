#include "game.h"
#include "../globals.h"
#include "../defs.h"
#include "../graphics/shader.h"
#include "../graphics/texture.h"
#include "../math/mat3.h"
#include "../math/math.h"

Game::Game()
    : numActors(0), worldW(50), worldH(40)
{
    player = spawnActor(Vec2(0.5f, 0.25f));
    //player->entity.rotation = 45.0f;
    camera.entity.pos = player->entity.pos;
    camera.entity.rotation = player->entity.rotation;
    for (int i = 0; i < 3; ++i)
    {
        spawnActor(Vec2(-0.7f + i * 0.4f, 0));
    }
    tileMap = new Tile[worldW * worldH];
    for (int y = 0; y < worldH; ++y)
    {
        for (int x = 0; x < worldW; ++x)
        {
            Tile *tile = &tileMap[x + y * worldW];
            tile->texture = thingyTexture;
            tile->xIndex = x;
            tile->yIndex = y;
        }
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

    /*if (g_input.isKeyPressed(KEY_LEFT))
    {
        player->entity.rotation += 0.15f;
    }
    if (g_input.isKeyPressed(KEY_RIGHT))
    {
        player->entity.rotation -= 0.15f;
    }*/

    player->entity.rotation -= g_input.mouseDeltaX * 0.3f;
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
    else if (player->entity.rotation < 360.0f)
    {
        player->entity.rotation += 360.0f;
    }

    camera.entity.pos = player->entity.pos + playerForward * 0.3f;
    camera.entity.rotation = player->entity.rotation;
}

static Mat3 view;
void Game::render() const
{

    view = Mat3::rotation(TO_RADIANS(-camera.entity.rotation)) * Mat3::translation(Vec2() - camera.entity.pos);

    drawTiles();
    for (int i = 0; i < numActors; ++i)
    {
        drawActor(actors[i]);
    }
}

void Game::drawActor(Actor *e) const
{
    g_renderer.renderVAO(e->vao, e->texture, Mat3::translation(e->entity.pos) * Mat3::rotation(TO_RADIANS(e->entity.rotation)), view);
}

const Tile *Game::getTileAtPos(Vec2 worldPos) const
{
    static float halfTile = 0.5f * TILE_SIZE;
    if (worldPos.x + halfTile < 0.0f || worldPos.y + halfTile < 0.0f) 
    {
        return nullptr;
    }
    unsigned int x = (unsigned int)((worldPos.x + halfTile) / TILE_SIZE);
    unsigned int y = (unsigned int)((worldPos.y + halfTile) / TILE_SIZE);
    if (x >= worldW || y >= worldH)
    {
        return nullptr;
    }
    return &tileMap[x + y * worldW];
}

void Game::drawTiles() const
{   
    Vec2 topLeft = camera.getTopLeft();
    Vec2 bottomLeft = camera.getBottomLeft();
    Vec2 topRight = camera.getTopRight();

    const Tile* topLeftTile = getTileAtPos(topLeft);
    const Tile* bottomLeftTile = getTileAtPos(bottomLeft);
    const Tile* topRightTile = getTileAtPos(topRight);
    /*
    if (topLeftTile)
    std::cout << "Top Left: " << topLeftTile->xIndex << ", " << topLeftTile->yIndex << std::endl;
    if (bottomLeftTile)
    std::cout << "Bottom Left: " << bottomLeftTile->xIndex << ", " << bottomLeftTile->yIndex << std::endl;
    if (topRightTile)
    std::cout << "Top Right: " << topRightTile->xIndex << ", " << topRightTile->yIndex << std::endl << std::endl;
    */
    unsigned int topTileY;
    unsigned int bottomTileY;
    if (topLeftTile) topTileY = topLeftTile->yIndex; 
    else topTileY = worldH - 1;
    if (bottomLeftTile) bottomTileY = bottomLeftTile->yIndex; 
    else bottomTileY = 0;

    //std::cout << topTileY - bottomTileY + 1 << std::endl;

    int yTileCount = (int)((topLeft.y - bottomLeft.y) / TILE_SIZE);
    
    //std::cout << yTileCount << std::endl;
    for (int y = 0; y < worldH; ++y)
    {
        for (int x = 0; x < worldW; ++x)
        {
            g_renderer.renderVAO(entityVAO, tileMap[x + y * worldW].texture, Mat3::translation(Vec2(TILE_SIZE * x, TILE_SIZE * y)), view);
        }
    }
}
#include "game.h"
#include "../globals.h"
#include "../defs.h"
#include "../graphics/shader.h"
#include "../graphics/texture.h"
#include "../math/mat3.h"
#include "../math/math.h"
#include "../graphics/renderer.h"

static Texture *redTex;
static Texture *greenTex;
static Tile voidTile;
static Quad debugQuad;

Game::Game()
    : numActors(0), worldW(50), worldH(40)
{
    debugQuad = Quad(-0.01f, -0.01f, -0.01f, 0.01f, 0.01f, 0.01f, 0.01f, -0.01f);
    voidTile.texture = nullptr;
    voidTile.xIndex = -1;
    voidTile.yIndex = -1;
    redTex = Texture::loadTexture("res/textures/red.bmp");
    greenTex = Texture::loadTexture("res/textures/green.bmp");
    player = spawnActor(Vec2(3.5f, 3.25f));
    //player->entity.rotation = 45.0f;
    camera.entity.pos = player->entity.pos;
    camera.entity.rotation = 0;
    currentCamera = &camera;
    debugCamera.entity.pos = player->entity.pos;
    for (int i = 0; i < 3; ++i)
    {
        spawnActor(Vec2(0.7f + i * 0.4f, 0));
    }
    tileMap = new Tile[worldW * worldH];
    for (int y = 0; y < worldH; ++y)
    {
        for (int x = 0; x < worldW; ++x)
        {
            Tile *tile = &tileMap[x + y * worldW];
            tile->texture = g_thingyTexture;
            tile->xIndex = x;
            tile->yIndex = y;
        }
    }
}

Game::~Game()
{
    delete redTex;
    redTex = nullptr;
    delete greenTex;
    greenTex = nullptr;
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
    e->speed = 0.002f;
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
    //std::cout << player->entity.rotation << std::endl;
    //debugCamera.entity.rotation = player->entity.rotation;
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

    if (g_input.isKeyPressed(KEY_LEFT))
    {
        debugCamera.entity.pos.x -= player->speed;
    }

    if (g_input.isKeyPressed(KEY_RIGHT))
    {
        debugCamera.entity.pos.x += player->speed;
    }

    if (g_input.isKeyPressed(KEY_UP))
    {
        debugCamera.entity.pos.y += player->speed;
    }
    if (g_input.isKeyPressed(KEY_DOWN))
    {
        debugCamera.entity.pos.y -= player->speed;
    }

    for (int i = 0; i < numActors; ++i)
    {
        Actor *e = actors[i];
        e->entity.pos += e->entity.vel;
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

    camera.entity.pos = player->entity.pos + playerForward * 0.3f;
    camera.entity.rotation = player->entity.rotation;
}

static Mat3 view;
void Game::render()
{

    view = Mat3::rotation(TO_RADIANS(-currentCamera->entity.rotation)) * Mat3::translation(Vec2() - currentCamera->entity.pos);
    g_renderer->setView(view);
    drawTiles();
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

Tile *Game::getTileAtPos(Vec2 worldPos)
{
    int x = (int)((worldPos.x) / TILE_SIZE);
    int y = (int)((worldPos.y) / TILE_SIZE);
    if (x < 0 || y < 0 || x >= worldW || y >= worldH)
        return &voidTile;
    return &tileMap[x + y * worldW];
}

static unsigned int memHandles[4];
static unsigned int currentMemHandle;

TileRasterBuffer Game::writeVecToTileRasterBuffer(Vec2 startPoint, Vec2 endPoint, bool start)
{
    Vec2 worldSize = getWorldAbsSize();

    Vec2 finalStartPoint;
    Vec2 finalEndPoint;

    if (startPoint.x < 0 && endPoint.x < 0)
    {
        finalStartPoint = Vec2(0, 0);
        finalEndPoint = Vec2(0, 0);
    }

    Vec2 dir = endPoint - startPoint;

    bool startOutOfBounds = false;
    bool endOutOfBounds = false;
    if (startPoint.x < 0 && startPoint.y < 0)
    {
        finalStartPoint = Vec2(0, 0);
        startOutOfBounds = true;
    }
    if (endPoint.x > worldSize.x && endPoint.y > worldSize.y)
    {
        finalEndPoint = Vec2(worldSize.x, worldSize.y);
        endOutOfBounds = true;
    }

 if (!startOutOfBounds)
   {
        finalStartPoint = startPoint;
        if (startPoint.y < 0)
        {
            finalStartPoint.x = getXAtLinePoint(dir, 0);
        }     
        if (startPoint.x < 0)
        {
            finalStartPoint.y = getYAtLinePoint(dir, 0);
        }
   }
   if (!endOutOfBounds)
   {
        finalEndPoint = endPoint;
        if (endPoint.y > worldSize.y)
        {
            finalEndPoint.x = getXAtLinePoint(dir, worldSize.y);
        }
        if (endPoint.x > worldSize.x)
        {
            finalEndPoint.y = getYAtLinePoint(dir, worldSize.x);
        }
    }

    //static float halfTile = 0.5f * TILE_SIZE;
    int startTileX = (int)((finalStartPoint.x) / TILE_SIZE);
    int startTileY = (int)((finalStartPoint.y ) / TILE_SIZE);
    int endTileX = (int)((finalEndPoint.x) / TILE_SIZE);
    int endTileY = (int)((finalEndPoint.y) / TILE_SIZE);

    Vec2 vec = finalEndPoint - finalStartPoint;

    int yLen = endTileY - startTileY + 1;
    ASSERT(yLen > 0);
    int *buffer;
    memHandles[currentMemHandle++] = g_memory.reserve(sizeof(int) * yLen, (void**)&buffer);
    Vec4 color;
    if (start)
    {
        color = Vec4(0, 1, 0, 1);
    }
    else
    {
        color = Vec4(1, 1, 0, 1);
    }
    for (int y = 0; y < yLen; ++y)
    {
        Vec2 curScreenPos = finalStartPoint + Vec2(getXAtLinePoint(vec, + y * TILE_SIZE), y * TILE_SIZE);
        Tile *tile = getTileAtPos(curScreenPos);
        if (start && tile->xIndex < 0)
            tile = getTileAtPos(Vec2(0, curScreenPos.y));
        else if (!start && tile->xIndex > worldW - 1)
            tile = getTileAtPos(Vec2(worldW, curScreenPos.y));
        buffer[y] = tile->xIndex;
        g_renderer->submitQuad(debugQuad, curScreenPos, color);
    }
    return {buffer, yLen};
}

void Game::drawTiles()
{
    currentMemHandle = 0;
    Vec2 top;
    Vec2 left;
    Vec2 bottom;
    Vec2 right;

    float camRot = camera.entity.rotation;
    //  std::cout << camRot << std::endl;

    if (camRot >= 0.0f && camRot < 90.0f)
    {
        top = camera.getTopRight();
        right = camera.getBottomRight();
        bottom = camera.getBottomLeft();
        left = camera.getTopLeft();
    }
    else if (camRot >= 90.0f && camRot < 180.0f)
    {
        top = camera.getBottomRight();
        right = camera.getBottomLeft();
        bottom = camera.getTopLeft();
        left = camera.getTopRight();
    }
    else if (camRot >= 180.0f && camRot < 270.0f)
    {
        top = camera.getBottomLeft();
        right = camera.getTopLeft();
        bottom = camera.getTopRight();
        left = camera.getBottomRight();
    }
    else
    {
        top = camera.getTopLeft();
        right = camera.getTopRight();
        bottom = camera.getBottomRight();
        left = camera.getBottomLeft();
    }

    Tile *topTile = getTileAtPos(top);
    Tile *leftTile = getTileAtPos(left);
    Tile *rightTile = getTileAtPos(right);
    Tile *bottomTile = getTileAtPos(bottom);

    int topTileY = topTile->yIndex;
    int bottomTileY = bottomTile->yIndex;
    int leftTileY = leftTile->yIndex;
    int rightTileY = rightTile->yIndex;

    TileRasterBuffer xStartBuffer0 = writeVecToTileRasterBuffer(bottom, left, true);
    TileRasterBuffer xEndBuffer0 = writeVecToTileRasterBuffer(bottom, right, false);
    TileRasterBuffer xStartBuffer1 = writeVecToTileRasterBuffer(left, top, true);
    TileRasterBuffer xEndBuffer1 = writeVecToTileRasterBuffer(right, top, false);

    int totalBufferSize = (xStartBuffer0.yLength + xStartBuffer1.yLength) * 2;
    int *combinedBuffer = new int[totalBufferSize];

    int xStartIndex = 0;
    for (int i = 0; i < xStartBuffer0.yLength; ++i)
    {
        combinedBuffer[(xStartIndex + i) * 2 + 0] = xStartBuffer0.buffer[i];
    }
    xStartIndex += xStartBuffer0.yLength;
    for (int i = 0; i < xStartBuffer1.yLength; ++i)
    {
        combinedBuffer[(xStartIndex + i) * 2 + 0] = xStartBuffer1.buffer[i];
    }

    int xEndIndex = 0;
    for (int i = 0; i < xEndBuffer0.yLength; ++i)
    {
        combinedBuffer[(xEndIndex + i) * 2 + 1] = xEndBuffer0.buffer[i];
    }
    xEndIndex += xEndBuffer0.yLength;
    for (int i = 0; i < xEndBuffer1.yLength; ++i)
    {
        combinedBuffer[(xEndIndex + i) * 2 + 1] = xEndBuffer1.buffer[i];
    }
    /*delete[] xStartBuffer0.buffer;
    delete[] xEndBuffer0.buffer;
    delete[] xStartBuffer1.buffer;
    delete[] xEndBuffer1.buffer;
*/
    for (int i = 0; i < 4; ++i)
    {
        g_memory.release(memHandles[i]);
    }
    for (int y = 0; y < totalBufferSize / 2; ++y)
    {
        for (int x = combinedBuffer[y * 2]; x < combinedBuffer[y * 2 + 1] + 1; ++x)
        {
            if (x < 0)
                continue;
            if (x >= worldW)
                break;
            ASSERT(x > -1);
            g_renderer->renderVAO(g_entityVAO, tileMap[x + (bottomTileY + y) * worldW].texture, 
            Mat3::translation(Vec2(TILE_SIZE * x - HALF_TILE_SIZE, TILE_SIZE * (bottomTileY + y) - HALF_TILE_SIZE)), view);
        }
    }
    //g_renderer->submitQuad(debugQuad, Vec2(-0.5f, 0.1f));
    g_renderer->submitLine(top, right, {0, 0});
    g_renderer->submitLine(right, bottom, {0, 0});
    g_renderer->submitLine(bottom, left, {0, 0});
    g_renderer->submitLine(left, top, {0, 0});
  //  g_renderer->submitQuad(debugQuad, top, Vec4(1, 0, 0, 1));
  //  g_renderer->submitQuad(debugQuad, left, Vec4(0, 1, 0, 1));
  //  g_renderer->submitQuad(debugQuad, bottom, Vec4(0, 0, 1, 1));
  //  g_renderer->submitQuad(debugQuad, right, Vec4(1, 0, 1, 1));
    delete[] combinedBuffer;
}

Vec2 Game::getWorldAbsSize() const
{
    return Vec2(worldW * TILE_SIZE, worldH * TILE_SIZE);
}
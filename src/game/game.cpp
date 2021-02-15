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
static Texture* wallTex;
static Tile voidTile;
static Tile thingyTile;
static Tile wallTile;
static Quad debugQuad;
static float speed;
static float accSpeed;


Game::Game()
    : numActors(0), worldW(50), worldH(40)
{
    speed = 1.0f;
    accSpeed = speed * 2;
    debugQuad = Quad(-0.01f, -0.01f, -0.01f, 0.01f, 0.01f, 0.01f, 0.01f, -0.01f);
    voidTile.texture = nullptr;
    //voidTile.xIndex = -1;
    //voidTile.yIndex = -1;
    voidTile.barrier = true;
    thingyTile.texture = g_thingyTexture;
    thingyTile.barrier = false;
    redTex = Texture::loadTexture("res/textures/red.bmp");
    greenTex = Texture::loadTexture("res/textures/green.bmp");
    wallTex = Texture::loadTexture("res/textures/walltile.bmp");
    wallTile.barrier = true;
    wallTile.texture = wallTex;
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
    tileMap = new Tile*[worldW * worldH];
    for (int y = 0; y < worldH; ++y)
    {
        for (int x = 0; x < worldW; ++x)
        {
            int index = x + y * worldW;
            if (y % 5 == 0 && x % 8 == 0)
            {
                tileMap[index] = &wallTile;
            }
            else
            {
                tileMap[x + y * worldW] = &thingyTile;
            }
        }
    }
}

Game::~Game()
{
    g_memory.release(redTex);
    g_memory.release(greenTex);
    g_memory.release(wallTex);
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
            Vec2 newVel = checkTileCollision(e->entity.pos, e->entity.vel * g_frameTime);
            e->entity.pos += newVel;
        }
    }

    Tile* currentPlayerTile = getTileAtPos(player->entity.pos);

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
    return (getTile(x, y));
}

Vec2 Game::getTileIndexAt(Vec2 worldPos)
{
    Vec2 result(worldPos.x / TILE_SIZE, worldPos.y / TILE_SIZE);
    return result;
}

Tile* Game::getTile(int x, int y)
{
    if (x < 0 || y < 0 || x >= worldW || y >= worldH)
    {
        return &voidTile;
    }
    return tileMap[x + y * worldW];
}

Vec2 Game::checkTileCollision(Vec2 pos, Vec2 vel)
{
    Vec2 nextPos = pos + vel;
    Tile* tile = getTileAtPos(nextPos);
    if (!tile->barrier)
    {
        return vel;
    }
    float newXVel;
    float newYVel;
    Tile* xTile = getTileAtPos(Vec2(nextPos.x, pos.y));
    Tile* yTile = getTileAtPos(Vec2(pos.x, nextPos.y));
    if (!xTile->barrier) newXVel = vel.x;
    else
    {
        int xDir = vel.x > 0 ? 0 : 1;
        
        newXVel = ((int)((nextPos.x + xDir * TILE_SIZE)/ TILE_SIZE ) * TILE_SIZE) - pos.x - !xDir * 0.001f;
        
    }
    if (!yTile->barrier) newYVel = vel.y;
    else
    {
        int yDir = vel.y > 0 ? 0 : 1;
        
        newYVel = ((int)((nextPos.y + yDir * TILE_SIZE)/ TILE_SIZE) * TILE_SIZE) - pos.y - !yDir * 0.001f;
        
    }
    return Vec2(newXVel, newYVel);
}
        

TileRasterBuffer Game::writeVecToTileRasterBuffer(Vec2 startPoint, Vec2 endPoint, bool start)
{
    Vec2 worldSize = getWorldAbsSize();

    Vec2 dir = endPoint - startPoint;


    int startTileX = (int)((startPoint.x) / TILE_SIZE);
    int startTileY = (int)((startPoint.y ) / TILE_SIZE);
    int endTileX = (int)((endPoint.x) / TILE_SIZE);
    int endTileY = (int)((endPoint.y) / TILE_SIZE);

    int yLen = (abs)(endTileY - startTileY + 2);
    
    ASSERT(yLen > 0);
    int *buffer;
    buffer = (int*)g_memory.reserve(sizeof(int) * yLen);
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
        Vec2 curScreenPos = startPoint + Vec2(getXAtLinePoint(dir, + y * TILE_SIZE), y * TILE_SIZE);
        int xIndex = (int)(curScreenPos.x / TILE_SIZE);
        buffer[y] = xIndex;
        g_renderer->submitQuad(debugQuad, curScreenPos, color);
    }
    return {buffer, yLen,};
}

void Game::drawTiles()
{
    Vec2 top;
    Vec2 left;
    Vec2 bottom;
    Vec2 right;

    float camRot = camera.entity.rotation;


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

    Vec2 topTile = getTileIndexAt(top);
    Vec2 leftTile = getTileIndexAt(left);
    Vec2 rightTile = getTileIndexAt(right);
    Vec2 bottomTile = getTileIndexAt(bottom);

    int topTileY = (int)topTile.y;
    int bottomTileY = (int)bottomTile.y;
    int leftTileY = (int)leftTile.y;
    int rightTileY = (int)rightTile.y;

    TileRasterBuffer xStartBuffer0 = writeVecToTileRasterBuffer(bottom, left, true);
    TileRasterBuffer xEndBuffer0 = writeVecToTileRasterBuffer(bottom, right, false);
    TileRasterBuffer xStartBuffer1 = writeVecToTileRasterBuffer(left, top, true);
    TileRasterBuffer xEndBuffer1 = writeVecToTileRasterBuffer(right, top, false);

    int totalBufferSize = (xStartBuffer0.yLength + xStartBuffer1.yLength) * 2;

    ASSERT(totalBufferSize > 0);
    int* combinedBuffer = (int*)g_memory.reserve(sizeof(int) * totalBufferSize);
    //int *combinedBuffer = new int[totalBufferSize];

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
    
    g_memory.release(xStartBuffer0.buffer);
    g_memory.release(xEndBuffer0.buffer);
    g_memory.release(xStartBuffer1.buffer);
    g_memory.release(xEndBuffer1.buffer);

    for (int y = 0; y < totalBufferSize / 2; ++y)
    {
        int yIndex = bottomTileY + y;
        if (yIndex < 0) continue;
        if (yIndex >= worldH) break;
        for (int x = combinedBuffer[y * 2]; x < combinedBuffer[y * 2 + 1] + 2; ++x)
        {
            if (x < 0)
                continue;
            if (x >= worldW)
                break;
            ASSERT(x > -1);
            
            Tile* tile = getTile(x, bottomTileY + y);
//            ASSERT(tile->xIndex >= 0);
//            ASSERT(tile->yIndex >= 0);
            ASSERT(tile->texture);
            
            g_renderer->renderVAO(g_entityVAO, tile->texture, 
            Mat3::translation(Vec2(TILE_SIZE * x + HALF_TILE_SIZE,
                                   TILE_SIZE * yIndex + HALF_TILE_SIZE)), view);
        }
    }
#ifdef DEBUG
    g_renderer->submitLine(top, right, {0, 0});
    g_renderer->submitLine(right, bottom, {0, 0});
    g_renderer->submitLine(bottom, left, {0, 0});
    g_renderer->submitLine(left, top, {0, 0});
 #endif 
    g_memory.release(combinedBuffer);
}

Vec2 Game::getWorldAbsSize() const
{
    return Vec2(worldW * TILE_SIZE, worldH * TILE_SIZE);
}

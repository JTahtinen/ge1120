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

Game::Game()
    : numActors(0), worldW(50), worldH(40)
{
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
    //std::cout << player->entity.rotation << std::endl;
    debugCamera.entity.rotation = player->entity.rotation;
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

    //drawTiles();
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
    static float halfTile = 0.5f * TILE_SIZE;

    int x = (int)((worldPos.x + halfTile) / TILE_SIZE);
    int y = (int)((worldPos.y + halfTile) / TILE_SIZE);
    if (x < 0 || y < 0 || x >= worldW || y >= worldH)
        return &voidTile;
    return &tileMap[x + y * worldW];
}

static int trbYLen;

TileRasterBuffer Game::writeVecToTileRasterBuffer(Vec2 startPoint, Vec2 endPoint, bool start)
{
    Vec2 worldSize = getWorldAbsSize();
    if (startPoint.y > worldSize.y)
        return {0, 0};
    if (endPoint.y < 0)
        return {0, 0};

    Vec2 finalStartPoint;
    Vec2 finalEndPoint;

    if (startPoint.x < 0 && endPoint.x < 0)
    {
        finalStartPoint = Vec2(0, 0);
        finalEndPoint = Vec2(0, 0);
    }
    
    Vec2 dir = endPoint - startPoint;


    bool outOfBounds = false;
    if (startPoint.x < 0 && startPoint.y < 0)
    {
        finalStartPoint = Vec2(0, 0);
        outOfBounds = true;
    }
    if (endPoint.x > worldSize.x && endPoint.y > worldSize.y)
    {
        finalEndPoint = Vec2(worldSize.x - TILE_SIZE, worldSize.y - TILE_SIZE);
        outOfBounds = true;
    }
    
    if (!outOfBounds)
    {
        finalStartPoint = startPoint;
        finalEndPoint = endPoint;
        if (startPoint.y < 0)
        {
            finalStartPoint = Vec2(getXAtLinePoint(dir, 0), 0);
        }     
        if (startPoint.x < 0)
        {
            finalStartPoint = Vec2(0, getYAtLinePoint(dir, 0));
        }
        if (endPoint.y > worldSize.y)
        {
            finalEndPoint = (Vec2(getXAtLinePoint(dir, worldSize.y - TILE_SIZE), worldSize.y - TILE_SIZE));
        }
        if (endPoint.x > worldSize.x)
        {
            finalEndPoint = (Vec2(worldSize.x - TILE_SIZE, getYAtLinePoint(dir, worldSize.x - TILE_SIZE)));
        }
    }

    
    static float halfTile = 0.5f * TILE_SIZE;
    int startTileX = (int)((finalStartPoint.x + halfTile) / TILE_SIZE);
    int startTileY = (int)((finalStartPoint.y + halfTile) / TILE_SIZE);
    int endTileX = (int)((finalEndPoint.x + halfTile) / TILE_SIZE);
    int endTileY = (int)((finalEndPoint.y + halfTile) / TILE_SIZE);
    
    Vec2 vec = finalEndPoint - finalStartPoint;
    /*
    if (startTileX < 0) startTileX = 0;
    else if (startTileX > worldW - 1) startTileX = worldW - 1;
    if (startTileY < 0) startTileY = 0;
    else if (startTileY > worldH - 1) startTileY = worldH - 1;
*/
    int yLen = endTileY - startTileY + 1;
    ASSERT(yLen > 0);
    int *buffer = new int[yLen];
    for (int y = 0; y < yLen; ++y)
    {
        Vec2 curScreenPos = finalStartPoint + Vec2(getXAtLinePoint(vec, (finalStartPoint.y + y * TILE_SIZE)), (finalStartPoint.y + y) * TILE_SIZE);

        Tile *tile = getTileAtPos(curScreenPos);
        if (start && tile->xIndex < 0)
            tile = getTileAtPos(Vec2(0, curScreenPos.y));
        else if (!start && tile->xIndex > worldW - 1)
            tile = getTileAtPos(Vec2(worldW, curScreenPos.y));
        buffer[y] = tile->xIndex;
    }
    return {buffer, yLen};
}

void Game::drawTiles()
{

    /*if (camera.entity.pos.x < 1.0f || camera.entity.pos.y < 1.0f 
    || camera.entity.pos.x > worldW * TILE_SIZE + 1.0f || 
    camera.entity.pos.y > worldH * TILE_SIZE + 1.0f)
    {
        return;
    }*/
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

    /*
    if (topLeftTile)
    std::cout << "Top Left: " << topLeftTile->xIndex << ", " << topLeftTile->yIndex << std::endl;
    if (bottomLeftTile)
    std::cout << "Bottom Left: " << bottomLeftTile->xIndex << ", " << bottomLeftTile->yIndex << std::endl;
    if (topRightTile)
    std::cout << "Top Right: " << topRightTile->xIndex << ", " << topRightTile->yIndex << std::endl << std::endl;
    */
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
    delete[] xStartBuffer0.buffer;
    delete[] xEndBuffer0.buffer;
    delete[] xStartBuffer1.buffer;
    delete[] xEndBuffer1.buffer;
    /*if (yDiff > 0 && yDiff < 20)
    {
        for (int i = 0; i < yDiff + 1; ++i)
        {

            Vec2 *startPoint;
            Vec2 *endPoint;
            Vec2 *startVec;
            Vec2 *endVec;
            if (i < yDiffMidBottomStart)
            {
                startPoint = &bottom;
                startVec = &startVec1;
            }
            else
            {
                startPoint = &left;
                startVec = &startVec0;
            }
            if (i < yDiffMidBottomEnd)
            {
                endPoint = &bottom;
                endVec = &endVec1;
            }
            else
            {
                endPoint = &right;
                endVec = &endVec0;
            }


            Tile *xStartTile =
                getTileAtPos(
                    Vec2(startPoint->x + getXAtLinePoint(*startVec, ((float)(bottomTileY + i) * TILE_SIZE)),
                         (float)(bottomTileY + i) * TILE_SIZE));
            //xStartTileNum = (int)((getXAtLinePoint(topLBotL, (bottomLeft.y + (float)i * TILE_SIZE)) + bottomLeft.x + (TILE_SIZE * 0.5f)) * (1.0f / TILE_SIZE));

            Tile *xEndTile = getTileAtPos(
                Vec2(endPoint->x + getXAtLinePoint(*endVec, ((float)(bottomTileY + i) * TILE_SIZE)),
                     (float)(bottomTileY + i) * TILE_SIZE));
            if (xStartTile)
            {
                tileRasterBuffer[i * 2 + 0] = xStartTile->xIndex;
                xStartTile->texture = greenTex;
                debugTiles[numDebugTiles++] = xStartTile;
            }

            if (xEndTile)
            {
                tileRasterBuffer[i * 2 + 1] = xEndTile->xIndex;
                xEndTile->texture = redTex;
                debugTiles[numDebugTiles++] = xEndTile;
                //std::cout << numDebugTiles << std::endl;
            }
        }
    }
*/
    //std::cout << topTileY - bottomTileY + 1 << std::endl;

    //std::cout << yTileCount << std::endl;
    for (int y = 0; y < totalBufferSize / 2; ++y)
    {
        for (int x = combinedBuffer[y * 2]; x < combinedBuffer[y * 2 + 1]; ++x)
        {
            if (x < 0)
                continue;
            if (x > worldW - 1)
                break;
            g_renderer->renderVAO(g_entityVAO, tileMap[x + (bottomTileY + y) * worldW].texture, Mat3::translation(Vec2(TILE_SIZE * x, TILE_SIZE * y)), view);
        }
    }
    delete[] combinedBuffer;
}

Vec2 Game::getWorldAbsSize() const
{
    return Vec2(worldW * TILE_SIZE, worldH * TILE_SIZE);
}
/*
void Game::drawTiles()
{
    Vec2 topLeft = camera.getTopLeft();
    Vec2 bottomLeft = camera.getBottomLeft();
    Vec2 topRight = camera.getTopRight();
    Vec2 bottomRight = camera.getBottomRight();
    Vec2 startVec;
    Vec2 endVec;
    if (topLeft.x > bottomRight.x)
    {
        if (topLeft.y > bottomLeft.y)
        {
            Vec2 temp = topRight;
            topRight = topLeft;
            topLeft = temp;
            temp = bottomRight;
            bottomRight = bottomLeft;
            bottomLeft = temp;
            startVec = topLeft - bottomLeft;
            endVec = topRight - bottomRight;
        }
        else
        {
            Vec2 temp = bottomRight;
            bottomRight = topLeft;
            topLeft = temp;
            temp = topRight;
            topRight = bottomLeft;
            bottomLeft = temp;
            startVec = bottomLeft - bottomRight;
            endVec = topLeft - topRight;
        }
    }
    else 
    {
        startVec = topLeft - bottomLeft;
        endVec = topRight - bottomRight;
    }

    const Tile *topLeftTile = getTileAtPos(topLeft);
    const Tile *bottomLeftTile = getTileAtPos(bottomLeft);
    const Tile *topRightTile = getTileAtPos(topRight);
    const Tile *bottomRightTile = getTileAtPos(bottomRight);
    
    if (topLeftTile)
    std::cout << "Top Left: " << topLeftTile->xIndex << ", " << topLeftTile->yIndex << std::endl;
    if (bottomLeftTile)
    std::cout << "Bottom Left: " << bottomLeftTile->xIndex << ", " << bottomLeftTile->yIndex << std::endl;
    if (topRightTile)
    std::cout << "Top Right: " << topRightTile->xIndex << ", " << topRightTile->yIndex << std::endl << std::endl;
    
    unsigned int topTileY;
    unsigned int bottomTileY;
    if (topLeftTile)
        topTileY = topLeftTile->yIndex;
    else
        topTileY = worldH - 1;
    if (bottomLeftTile)
        bottomTileY = bottomLeftTile->yIndex;
    else
        bottomTileY = 0;


    unsigned int yDiff = topTileY - bottomTileY;
    unsigned int xStartTileNum;
    Tile *debugTiles[40];
    unsigned int numDebugTiles = 0;
    if (yDiff > 0 && yDiff < 20)
    {
        for (unsigned int i = 0; i < yDiff + 1; ++i)
        {
            Tile *xStartTile =
                getTileAtPos(
                    Vec2(bottomLeft.x + getXAtLinePoint(startVec, ((float)i * TILE_SIZE)),
                         bottomLeft.y + (float)i * TILE_SIZE));
            //xStartTileNum = (int)((getXAtLinePoint(topLBotL, (bottomLeft.y + (float)i * TILE_SIZE)) + bottomLeft.x + (TILE_SIZE * 0.5f)) * (1.0f / TILE_SIZE));

            Tile *xEndTile = getTileAtPos(
                Vec2(bottomRight.x + getXAtLinePoint(endVec, ((float)i * TILE_SIZE)),
                     bottomRight.y + (float)i * TILE_SIZE));
            if (xStartTile)
            {
                xStartTile->texture = redTex;
                if (g_input.isKeyTyped(KEY_K))
                {
                    // std::cout << xStartTile->xIndex << ", " << xStartTileNum << std::endl;
                }
                debugTiles[numDebugTiles++] = xStartTile;
            }
            if (xEndTile)
            {
                xEndTile->texture = redTex;
                if (g_input.isKeyTyped(KEY_K))
                {
                    // std::cout << xStartTile->xIndex << ", " << xStartTileNum << std::endl;
                }
                debugTiles[numDebugTiles++] = xEndTile;
                //std::cout << numDebugTiles << std::endl;
            }
        }
    }

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
    for (unsigned int i = 0; i < numDebugTiles; ++i)
    {
        debugTiles[i]->texture = thingyTexture;
    }
}*/
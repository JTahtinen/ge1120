#include "tilemap.h"
#include "../graphics/texture.h"
#include "../graphics/renderer.h"
#include "../math/math.h"
#include "../defs.h"
#include "../globals.h"

static Quad debugQuad;
TileMap::~TileMap()
{
    g_memory.release(tiles);
    for (int i = 0; i < 4; ++i)
    {
        g_memory.release(tileRasterBuffer.elems[i].buffer);
    }
}

bool TileMap::init(int width, int height)
{
    for (int i = 0; i < 4; ++i)
    {
        tileRasterBuffer.elems[i].buffer = (int*)g_memory.reserve(sizeof(int)
                                                     * MAX_TILE_RASTER_BUFFER_YLEN);
    }
    debugQuad = Quad(-0.01f, -0.01f, -0.01f, 0.01f, 0.01f, 0.01f, 0.01f, -0.01f);
    if (width < 0 || height < 0)
    {
        err("Tilemap initialization - negative size, W: %d, H: %d!\n", width, height);
        return false;
    }
    if (width >= 400 || height >= 400)
    {
        err("Tilemap initialization - tilemap too big, W %d, H: %d!\n", width, height);
        return false;
    }
    this->width = width;
    this->height = height;
    tiles = (Tile**)g_memory.reserve(sizeof(Tile*) * width * height);
    return true;
}

Tile *TileMap::getTileAtPos(Vec2 worldPos)
{
    int x = floorToInt(worldPos.x / TILE_SIZE);
    int y = floorToInt(worldPos.y / TILE_SIZE);
    return (getTile(x, y));
}

iPoint TileMap::getTileIndexAt(Vec2 worldPos)
{
    iPoint result{floorToInt(worldPos.x / TILE_SIZE), floorToInt(worldPos.y / TILE_SIZE)};
    return result;
}

void TileMap::setTile(int x, int y, Tile* tile)
{
    if (!tile)
    {
        err("Could not set tile %d, %d - Tile was NULL!\n", x, y);
    }
    if (x >= 0 && x < width && y >= 0 && y < height)
    {
        tiles[x + y * width] = tile;
    }
}

Tile* TileMap::getTile(int x, int y)
{
    if (x < 0 || y < 0 || x >= width || y >= height)
    {
        return g_voidTile;
    }
    return tiles[x + y * width];
}

Vec2 TileMap::checkTileCollision(Vec2 pos, Vec2 vel)
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
        
void TileMap::writeVecToTileRasterBuffer(Vec2 startPoint, Vec2 endPoint, bool start,
                                         TileRasterBufferElement* target)
{
    Vec2 dir = (endPoint - startPoint).normalize();


    int startTileX = floorToInt(startPoint.x / TILE_SIZE);
    int startTileY = floorToInt(startPoint.y  / TILE_SIZE);
    int endTileX = floorToInt(endPoint.x / TILE_SIZE);
    int endTileY = floorToInt(endPoint.y / TILE_SIZE);

    int yLen = (abs)(endTileY - startTileY);

    if (yLen > MAX_TILE_RASTER_BUFFER_YLEN - 1) return;

    if (yLen > 50)
    {
        yLen = 50;
    }
    target->yLength = yLen;
    if (yLen == 0) return;
    yLen += 1;
    
    Vec4 debugQuadColor;
    if (start)
    {
        debugQuadColor = Vec4(0, 0, 1, 0.5f);
    }
    else
    {
        debugQuadColor = Vec4(1, 1, 0, 0.5f);
    }
    Vec2 curScreenPos = startPoint;
    for (int y = 0; y < yLen; ++y)
    {
        if (y == yLen - 1)
        {
            curScreenPos = endPoint;
        }
        else
        {
            curScreenPos = findVerticalTileIntersection(curScreenPos + dir * 0.00001f, dir);
        }
        int xIndex = floorToInt(curScreenPos.x / TILE_SIZE);

        target->buffer[y] = xIndex;
        if (g_debugMode)
        {
            g_uiRenderer->submitQuad(debugQuad, curScreenPos, debugQuadColor);
        }
  
    }
    target->yLength = yLen;
}

void TileMap::draw(Camera* camera, Mat3& view)
{
    Vec2 top;
    Vec2 left;
    Vec2 bottom;
    Vec2 right;

    float camRot = camera->entity.rotation;


    if (camRot >= 0.0f && camRot < 90.0f)
    {
        top = camera->getTopRight();
        right = camera->getBottomRight();
        bottom = camera->getBottomLeft();
        left = camera->getTopLeft();
    }
    else if (camRot >= 90.0f && camRot < 180.0f)
    {
        top = camera->getBottomRight();
        right = camera->getBottomLeft();
        bottom = camera->getTopLeft();
        left = camera->getTopRight();
    }
    else if (camRot >= 180.0f && camRot < 270.0f)
    {
        top = camera->getBottomLeft();
        right = camera->getTopLeft();
        bottom = camera->getTopRight();
        left = camera->getBottomRight();
    }
    else
    {
        top = camera->getTopLeft();
        right = camera->getTopRight();
        bottom = camera->getBottomRight();
        left = camera->getBottomLeft();
    }

    iPoint topTile = getTileIndexAt(top);
    iPoint leftTile = getTileIndexAt(left);
    iPoint rightTile = getTileIndexAt(right);
    iPoint bottomTile = getTileIndexAt(bottom);

    int topTileY = topTile.y;
    int bottomTileY = bottomTile.y;
    int leftTileY = leftTile.y;
    int rightTileY = rightTile.y;

    TileRasterBufferElement& xStartBuffer0 = tileRasterBuffer.xStartBuffer0;
    TileRasterBufferElement& xEndBuffer0 = tileRasterBuffer.xEndBuffer0;
    TileRasterBufferElement& xStartBuffer1 = tileRasterBuffer.xStartBuffer1;
    TileRasterBufferElement& xEndBuffer1 = tileRasterBuffer.xEndBuffer1;
    writeVecToTileRasterBuffer(bottom, left, true, &xStartBuffer0);
    writeVecToTileRasterBuffer(bottom, right, false, &xEndBuffer0);
    writeVecToTileRasterBuffer(left, top, true, &xStartBuffer1);
    writeVecToTileRasterBuffer(right, top, false, &xEndBuffer1);

    size_t totalBufferSize = (xStartBuffer0.yLength + xStartBuffer1.yLength + 1) * 2;

    ASSERT(totalBufferSize > 0);
    size_t bufferMemorySize = sizeof(int) * totalBufferSize;
    int* combinedBuffer = (int*)g_memory.reserve(bufferMemorySize);

    int xStartIndex = 0;
    for (int i = 0; i < xStartBuffer0.yLength; ++i)
    {
        ASSERT(i < MAX_TILE_RASTER_BUFFER_YLEN)
        combinedBuffer[(xStartIndex + i) * 2 + 0] = xStartBuffer0.buffer[i];
    }
    xStartIndex += xStartBuffer0.yLength;
    for (int i = 0; i < xStartBuffer1.yLength; ++i)
    {
        ASSERT(i < MAX_TILE_RASTER_BUFFER_YLEN)
        combinedBuffer[(xStartIndex + i) * 2 + 0] = xStartBuffer1.buffer[i];
    }

    int xEndIndex = 0;
    for (int i = 0; i < xEndBuffer0.yLength; ++i)
    {
        ASSERT(i < MAX_TILE_RASTER_BUFFER_YLEN)
        combinedBuffer[(xEndIndex + i) * 2 + 1] = xEndBuffer0.buffer[i];
    }
    xEndIndex += xEndBuffer0.yLength;
    for (int i = 0; i < xEndBuffer1.yLength; ++i)
    {
        ASSERT(i < MAX_TILE_RASTER_BUFFER_YLEN)
        combinedBuffer[(xEndIndex + i) * 2 + 1] = xEndBuffer1.buffer[i];
    }

    static Sprite tileSprite {NULL, Quad(-0.1f, -0.1f, -0.1f, 0.1f, 0.1f, 0.1f, 0.1f, -0.1f)};
   
    for (int y = 0; y < totalBufferSize / 2; ++y)
    {
        int yIndex = bottomTileY + y;
        if (yIndex < 0) continue;
        if (yIndex >= height) break;
        size_t bufferStartIndex = y * 2;
        size_t bufferEndIndex = bufferStartIndex + 1;
        for (int x = combinedBuffer[bufferStartIndex]; x < combinedBuffer[bufferEndIndex] + 1; ++x)
        {
            if (x < 0)
            {
                if (combinedBuffer[y * 2 + 1] <= 0) break;
                else x = 0;                                                                    
            }

            if (x >= width)
            {
                break;
            }
            ASSERT(x > -1);
            //ASSERT(x != 13 && y != 8);
            ASSERT(bufferStartIndex < bufferMemorySize);
            ASSERT(bufferEndIndex < bufferMemorySize);
            Tile* tile = getTile(x, bottomTileY + y);
            tileSprite.texture = tile->texture;
            ASSERT(tile->texture);
            g_renderer->submitSprite(&tileSprite,
                                     Mat3::translation(
                                         Vec2(TILE_SIZE * x + HALF_TILE_SIZE,
                                              TILE_SIZE * yIndex + HALF_TILE_SIZE)), view, false);
        }
    }
    if (g_debugMode)
    {
        g_renderer->submitLine(top, right, {0, 0});
        g_renderer->submitLine(right, bottom, {0, 0});
        g_renderer->submitLine(bottom, left, {0, 0});
        g_renderer->submitLine(left, top, {0, 0});
    }
    g_memory.release(combinedBuffer);
}

Vec2 TileMap::getWorldAbsSize() const
{
    return Vec2(width * TILE_SIZE, height * TILE_SIZE);
}

Vec2 TileMap::findTileIntersection(Vec2 pos, Vec2 dir)
{
    int currentTileY = floorToInt(pos.y / TILE_SIZE);
    float currentTileLeftEdge = (float)currentTileY * TILE_SIZE;
    float currentTileRightEdge = (float)(currentTileY + 1) * TILE_SIZE;
    Vec2 horizontalIntersection = findHorizontalTileIntersection(pos, dir);

    if (horizontalIntersection.y < (float)(currentTileY * TILE_SIZE)
        || horizontalIntersection.y > (float)(currentTileY + 1) * TILE_SIZE)
    {
        return findVerticalTileIntersection(pos, dir);
    }
    return horizontalIntersection;
}

Vec2 TileMap::findVerticalTileIntersection(Vec2 pos, Vec2 dir)
{
    int currentTileX = floorToInt(pos.x / TILE_SIZE);
    int currentTileY = floorToInt(pos.y / TILE_SIZE);
    Vec2 posInsideTile(fremainder(pos.x, TILE_SIZE), fremainder(pos.y, TILE_SIZE));
    float dirAngle = getAngleOfVec2(dir);
    float intersection = 0;
    float yEdge = 0;
    if (valIsBetween(dirAngle, 0, 90.0f))
    {
        intersection = posInsideTile.x + getXAtLinePoint(dir, TILE_SIZE - posInsideTile.y);
        yEdge = 1;
    }
    else if (valIsBetween(dirAngle, 90.0f, 180.0f))
    {
        intersection = posInsideTile.x + getXAtLinePoint(dir, TILE_SIZE - posInsideTile.y);
        yEdge = 1;
    }
    else if (valIsBetween(dirAngle, 180.0f, 270.f))
    {
        intersection = posInsideTile.x + getXAtLinePoint(dir, -posInsideTile.y);
        yEdge = 0;
    }
    else
    {
        intersection = posInsideTile.x + getXAtLinePoint(dir, -posInsideTile.y);
        yEdge = 0;
    }
    
    Vec2 result = Vec2((float)currentTileX * TILE_SIZE + intersection,
                       (float)currentTileY * TILE_SIZE + (yEdge * TILE_SIZE));
    return result;
}

Vec2 TileMap::findHorizontalTileIntersection(Vec2 pos, Vec2 dir)
{
    int currentTileX = floorToInt(pos.x / TILE_SIZE);
    int currentTileY = floorToInt(pos.y / TILE_SIZE);
    Vec2 posInsideTile(fremainder(pos.x, TILE_SIZE), fremainder(pos.y, TILE_SIZE));
    float dirAngle = getAngleOfVec2(dir);
    float intersection;
    float xEdge = 0;

    if (valIsBetween(dirAngle, 0, 90.0f))
    {
        intersection = posInsideTile.y + getYAtLinePoint(dir, TILE_SIZE - posInsideTile.x);
        xEdge = 1;
    }
    else if (valIsBetween(dirAngle, 90.0f, 180.0f))
    {
        intersection = posInsideTile.y + getYAtLinePoint(dir,-posInsideTile.x);
        xEdge = 0;

    }
    else if (valIsBetween(dirAngle, 180.0f, 270.f))
    {
        intersection = posInsideTile.y + getYAtLinePoint(dir, -posInsideTile.x);
        xEdge = 0;
    }
    else
    {
        intersection = posInsideTile.y + getYAtLinePoint(dir, TILE_SIZE - posInsideTile.x);
        xEdge = 1;
    }
    
    Vec2 result = Vec2((float)currentTileX * TILE_SIZE + (xEdge * TILE_SIZE),
                       (float)currentTileY * TILE_SIZE + intersection);        
    return result;
}


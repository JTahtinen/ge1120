#pragma once

#include "camera.h"
#include "../math/math.h"

struct Texture;

#define TILE_SIZE (0.2f)
#define HALF_TILE_SIZE (TILE_SIZE * 0.5f)
#define MAX_TILE_RASTER_BUFFER_YLEN (50)

struct Tile
{
    bool barrier;
    const Texture* texture;
};

struct TileRasterBufferElement
{
    int* buffer;
    int yLength;
    int yStart;
};

union TileRasterBuffer
{
    struct
    {
        TileRasterBufferElement xStartBuffer0;
        TileRasterBufferElement xEndBuffer0;
        TileRasterBufferElement xStartBuffer1;
        TileRasterBufferElement xEndBuffer1;
    };
    TileRasterBufferElement elems[4];
};

struct TileMap
{
    Tile** tiles;
    int width;
    int height;

    TileRasterBuffer tileRasterBuffer;
    
    ~TileMap();
    bool init(int width, int height);
    Tile* getTileAtPos(Vec2 worldPos);
    Vec2 getTileIndexAt(Vec2 worldPos);
    void setTile(int x, int y, Tile* tile);
    Tile* getTile(int x, int y);
    Vec2 checkTileCollision(Vec2 pos, Vec2 vel);
    Vec2 getWorldAbsSize() const;
    void draw(Camera* camera, Mat3& view);
    Vec2 findTileIntersection(Vec2 pos, Vec2 dir);
    float findHorizontalTileIntersection(Vec2 pos, Vec2 dir);
    float findVerticalTileIntersection(Vec2 pos, Vec2 dir);
private:
    void writeVecToTileRasterBuffer(Vec2 startPoint, Vec2 endPoint, bool start,
                                    TileRasterBufferElement* target);
    
};

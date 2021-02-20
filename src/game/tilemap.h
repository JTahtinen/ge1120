#pragma once

#include "camera.h"
#include "../math/math.h"

struct Texture;

#define TILE_SIZE (0.2f)
#define HALF_TILE_SIZE (TILE_SIZE * 0.5f)

struct Tile
{
    bool barrier;
    const Texture* texture;
};

struct TileRasterBuffer
{
    int* buffer;
    int yLength;
    int yStart;
};


struct TileMap
{
    Tile** tiles;
    int width;
    int height;

    ~TileMap();
    bool init(int width, int height);
    Tile* getTileAtPos(Vec2 worldPos);
    Vec2 getTileIndexAt(Vec2 worldPos);
    void setTile(int x, int y, Tile* tile);
    Tile* getTile(int x, int y);
    Vec2 checkTileCollision(Vec2 pos, Vec2 vel);
    Vec2 getWorldAbsSize() const;
    void draw(Camera* camera, Mat3& view);
private:
    TileRasterBuffer writeVecToTileRasterBuffer(Vec2 startPoint, Vec2 endPoint, bool start);
};

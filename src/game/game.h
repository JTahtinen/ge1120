#pragma once
#include "entity.h"
#include "camera.h"

class Shader;
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

#define MAX_GAME_ENTITIES (100)
struct Game
{
    int worldW {0};
    int worldH {0};
    Tile** tileMap;
    Actor* player;
    Actor* actors[MAX_GAME_ENTITIES];
    Actor actorPool[MAX_GAME_ENTITIES];
    unsigned int numActors;
    Camera camera;
    Camera debugCamera;
    Camera* currentCamera;


    Game();
    ~Game();
    void update();
    void render();
    private:
    void drawActor(Actor* e) const;
    Actor* spawnActor(Vec2 pos);
    Tile* getTileAtPos(Vec2 worldPos);
    Vec2 getTileIndexAt(Vec2 worldPos); 
    Tile* getTile(int x, int y);
    private:
    Vec2 checkTileCollision(Vec2 pos, Vec2 vel);
    void drawTiles();
    Vec2 getWorldAbsSize() const;
    TileRasterBuffer writeVecToTileRasterBuffer(Vec2 startPoint, Vec2 endPoint, bool start);
};

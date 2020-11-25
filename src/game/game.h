#pragma once
#include "entity.h"
#include "camera.h"

class Shader;
class Texture;

#define TILE_SIZE (0.2f)
struct Tile
{
    unsigned int xIndex;
    unsigned int yIndex;
    const Texture* texture;
};

#define MAX_GAME_ENTITIES (100)
struct Game
{
    unsigned int worldW {0};
    unsigned int worldH {0};
    Tile* tileMap;
    Actor* player;
    Actor* actors[MAX_GAME_ENTITIES];
    Actor actorPool[MAX_GAME_ENTITIES];
    unsigned int numActors;
    Camera camera;


    Game();
    void update();
    void render() const;
    private:
    void drawActor(Actor* e) const;
    Actor* spawnActor(Vec2 pos);
    const Tile* getTileAtPos(Vec2 worldPos) const;
    void drawTiles() const;
};
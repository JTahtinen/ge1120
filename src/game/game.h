#pragma once
#include "entity.h"

class Shader;
class Texture;

struct Tile
{
    const Texture* texture;
};

#define MAX_GAME_ENTITIES 100
struct Game
{
    unsigned int worldW {0};
    unsigned int worldH {0};
    Tile* tileMap;
    Entity* player;
    Entity* entities[MAX_GAME_ENTITIES];
    Entity entityPool[MAX_GAME_ENTITIES];
    unsigned int numEntities;

    Game();
    void update();
    void render() const;
    private:
    void drawEntity(Entity* e) const;
    Entity* spawnEntity(Vec2 pos);
};
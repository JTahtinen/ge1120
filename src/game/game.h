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
    Actor* player;
    Actor* actors[MAX_GAME_ENTITIES];
    Actor actorPool[MAX_GAME_ENTITIES];
    unsigned int numActors;
    Entity camera;


    Game();
    void update();
    void render() const;
    private:
    void drawActor(Actor* e) const;
    Actor* spawnActor(Vec2 pos);
};
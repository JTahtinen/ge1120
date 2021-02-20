#pragma once
#include "entity.h"
#include "camera.h"
#include "tilemap.h"

#define MAX_GAME_ENTITIES (100)
struct Game
{
    TileMap tileMap;
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
};

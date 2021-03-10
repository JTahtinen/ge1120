#pragma once
#include "entity.h"
#include "camera.h"
#include "tilemap.h"
#include "../math/mat3.h"
#include "../util/vector.h"
#include "../util/string.h"

#define MAX_GAME_ENTITIES (100)
struct DataStrings
{
    Vector<String> strings;
};

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


    void init();
    ~Game();
    void update();
    void render();
    private:
    void drawActor(Actor* e) const;
    Actor* spawnActor(Vec2 pos);    
};

extern Mat3 screenToWorldProjection(Game* game);
extern void getDataFromPos(Game* game, Vec2 pos, DataStrings* result);

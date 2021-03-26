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
    TileCollection tileCollection;
    Vector<TileMap> tileMaps;
    Actor* player;
    Actor* actors[MAX_GAME_ENTITIES];
    Actor actorPool[MAX_GAME_ENTITIES];
    unsigned int numActors;
    Camera camera;
    Camera debugCamera;
    Camera* currentCamera;
    Vector<Conversation> conversations;
    Actor* playerTarget;
    Conversation* activeConversation;
    bool actorEngaged;
    void init();
    ~Game();
    void update();
    void render();
    private:
    void drawActor(Actor* e) const;
    Actor* spawnActor(Vec2 pos);
    void engageActor(Actor* actor);
};

Mat3 screenToWorldProjection(Game* game);
void getDataFromPos(Game* game, Vec2 pos, DataStrings* result);
bool loadTileCollection(const char* filepath, TileCollection* collection);
TileMap* getTileMapAtWorldPos(Vec2 pos, Game* game);
Vec2 checkCollision(Vec2 pos, Vec2 vel, Game* game);

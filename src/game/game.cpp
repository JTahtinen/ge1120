#include "game.h"
#include "../globals.h"
#include "../defs.h"
#include "../graphics/shader.h"
#include "../graphics/texture.h"
#include "../math/mat3.h"
#include "../math/math.h"
#include "../graphics/renderer.h"
#include "../util/string.h"
#include "../util/textparser.h"
#include "../util/file.h"
#include "conversation.h"

static float speed;
static float accSpeed;
static Mat3 view;

void Game::init()
{
    conversations.push_back({});
    Conversation& conv = conversations.back();
    for (size_t i = 0; i < MAX_GAME_ENTITIES; ++i)
    {
        actorPool[i].entity.name.content = NULL;
    }
    loadConversation("res/test.conv", &conv);
    tileMaps.reserve(2);
    tileMaps.push_back({0});
    tileMaps.push_back({0});
    numActors = 0;
    speed = 1.0f;
    accSpeed = speed * 2;
    player = spawnActor(Vec2(0.4f, 0.4f));
    player->entity.name = "Player";
    camera.entity.pos = player->entity.pos;
    camera.entity.rotation = 0;
    camera.zoom = 4.0f;
    camera.zoomTarget = 1.0f;
    currentCamera = &camera;
    debugCamera.entity.pos = player->entity.pos;
    debugCamera.entity.rotation = 0;
    debugCamera.zoom = 1.0f;
    debugCamera.zoomTarget = 1.0f;
    activeConversation = NULL;
    playerTarget = NULL;
    for (int i = 0; i < 3; ++i)
    {
        spawnActor(Vec2(0.7f + i * 0.4f, 0.6f));
    }
    actors[1]->conv = &conv;

    loadTileCollection("res/basictiles.tc", &tileCollection);
    if (tileMaps[0].init(20, 10))
    {
        tileMaps[0].position = {0, 0};        
        for (int y = 0; y < tileMaps[0].height; ++y)
        {
            for (int x = 0; x < tileMaps[0].width; ++x)
            {
                tileMaps[0].setTile(x, y, getTile("Grass", &tileCollection)); 
            }
        }
        
    }
    if (tileMaps[1].init(5, 6))
    {
        tileMaps[1].position = {20, 2};
                
        for (int y = 0; y < tileMaps[1].height; ++y)
        {
            for (int x = 0; x < tileMaps[1].width; ++x)
            {
                tileMaps[1].setTile(x, y, getTile("Floor", &tileCollection)); 
            }
        }
        
    }
}


Game::~Game()
{
}

Actor *Game::spawnActor(Vec2 pos)
{
    if (numActors >= MAX_GAME_ENTITIES)
    {
        warn("Could not spawn actor - Limit reached!\n");
        return nullptr;
    }
    static Sprite entitySprite {g_entityTexture, Quad(-0.1f, -0.1f, -0.1f, 0.1f, 0.1f, 0.1f, 0.1f, -0.1f)};
    Actor *e = &actorPool[numActors];
    static unsigned int currentActorID = 0;
    e->entity.id = currentActorID++;
    e->entity.name = "Thingy " + toString(e->entity.id);
    e->entity.pos = pos;
    e->entity.rotation = 0.0f;
    e->sprite = &entitySprite;
    e->speed = speed;
    actors[numActors++] = e;
    return e;
}

void Game::engageActor(Actor* actor)
{
    if (actor->conv)
    {
        activeConversation = actor->conv;
    }
}

void Game::update()
{

    if (activeConversation)
    {
        bool conversationEnded = false;
        if (g_input.isKeyTyped(KEY_E))
        {
            conversationEnded = true;
        }
        if (!conversationEnded && updateConversation(activeConversation) == CONV_ACTIVE)
        {            
            for (unsigned int i = 0; i < 9; ++i)
            {                
                if (g_input.isKeyTyped(KEY_1 + i))
                {
                    chooseOption(i + 1, activeConversation);
                }          
            }
        }
        else
        {
            conversationEnded = true;
        }
        
        if (conversationEnded)
        {
            reset(activeConversation);
            activeConversation = NULL;
        }
    }
    else
    {
        player->entity.vel *= 0;
        if (!g_mouseState)
        {
            player->entity.rotation -= g_input.mouseDeltaX * 30.0f * g_frameTime;
        }
        Mat3 playerRotationMat = Mat3::rotation(TO_RADIANS(player->entity.rotation));
        Vec2 playerForward = playerRotationMat * Vec2(0, 1.0f);
        Vec2 playerRight(playerForward.y, -playerForward.x);
        if (g_input.isKeyPressed(KEY_A))
        {
            player->entity.vel -= playerRight * player->speed;
        }
        if (g_input.isKeyPressed(KEY_D))
        {
            player->entity.vel += playerRight * player->speed;
        }
        if (g_input.isKeyPressed(KEY_W))
        {
            player->entity.vel += playerForward * player->speed;
        }
        if (g_input.isKeyPressed(KEY_S))
        {
            player->entity.vel -= playerForward * player->speed;
        }
        if (g_input.isKeyPressed(KEY_LSHIFT))
        {
            player->speed = accSpeed;
        }
        else
        {
            player->speed = speed;
        }
        if (g_input.isKeyTyped(KEY_K))
        {
            if (currentCamera == &camera)
            {
                currentCamera = &debugCamera;
                debugCamera.entity.pos = player->entity.pos;
            }
            else
                currentCamera = &camera;
        }

        float playerFrameVel = player->speed * g_frameTime;

        if (g_input.isKeyPressed(KEY_LEFT))
        {
            debugCamera.entity.pos.x -= playerFrameVel;
        }

        if (g_input.isKeyPressed(KEY_RIGHT))
        {
            debugCamera.entity.pos.x += playerFrameVel;
        }

        if (g_input.isKeyPressed(KEY_UP))
        {
            debugCamera.entity.pos.y += playerFrameVel;
        }
        if (g_input.isKeyPressed(KEY_DOWN))
        {
            debugCamera.entity.pos.y -= playerFrameVel;
        }
        if (g_input.mouseWheelDown)
        {
            increaseZoom(currentCamera);
        }
        if (g_input.mouseWheelUp)
        {
            decreaseZoom(currentCamera);
        }
        for (int i = 0; i < numActors; ++i)
        {
            Actor *e = actors[i];
            if (e->entity.vel.length() > 0)
            {
                //TODO: For all tilemaps
                Vec2 newVel = checkCollision(e->entity.pos, e->entity.vel * g_frameTime, this);
                e->entity.pos += newVel;
            }
        }

        Tile* currentPlayerTile = tileMaps[0].getTileAtPos(player->entity.pos);    

        while (player->entity.rotation > 360.0f)
        {
            player->entity.rotation -= 360.0f;
        }

        while (player->entity.rotation < 0)
        {
            player->entity.rotation += 360.0f;
        }

        camera.entity.pos = player->entity.pos + playerForward * 0.3f * g_frameTime;
        camera.entity.rotation = player->entity.rotation;
        updateCamera(currentCamera);
        if (g_debugMode)
        {
            g_uiRenderer->submitText("Player Pos: X - " + toString(player->entity.pos.x) +
                                     ", Y - " + toString(player->entity.pos.y), Vec2(-0.1f, -0.3f));
        }

        bool playerTargetFound = false;
        float acceptableTargetDistance = 0.4f;
        for (unsigned int i = 1; i < numActors; ++i)
        {

            float distance = (actors[i]->entity.pos - player->entity.pos).length();
        
            if (distance < acceptableTargetDistance)
            {
                acceptableTargetDistance = distance;
                playerTarget = actors[i];
                playerTargetFound = true;
            }
        }
        if (!playerTargetFound)
        {
            playerTarget = NULL;
        }
        if (!playerTarget)
        {
            actorEngaged = false;
        }
        if (g_input.isKeyTyped(KEY_E))
        {
            if (playerTarget)
            {
                actorEngaged = !actorEngaged;
            }
        }
        if (actorEngaged)
        {
            engageActor(playerTarget);
            actorEngaged = false;
        }
    }
}


void Game::render()
{

    view = Mat3::view(Vec2() - currentCamera->entity.pos,
                      TO_RADIANS(-currentCamera->entity.rotation),
                      1.0f / currentCamera->zoom,
                      g_aspect);  
    g_renderer->setView(view);
    if (playerTarget)
    {
        g_uiRenderer->submitText(playerTarget->entity.name, playerTarget->entity.pos + Vec2(0, 0.1f), view, 0.2f);
    }
    for (size_t i = 0; i < tileMaps.size; ++i)
    {
        tileMaps[i].draw(&camera, view);
    }
    for (int i = 0; i < numActors; ++i)
    {
        drawActor(actors[i]);
    }

    
    static float angle = 350;
    Vec2 dir = createVec2FromAngle(angle);
    if (g_input.isKeyPressed(KEY_O))
    {
        angle -= 80.0f * g_frameTime;
    }
    if (g_input.isKeyPressed(KEY_P))
    {
        angle += 80.0f * g_frameTime;
    }
    if (g_debugMode)
    {
        Vec2 intersection = tileMaps[0].findTileIntersection(player->entity.pos, dir);
        Quad quad = Quad(-0.01f, -0.01f, -0.01f, 0.01f, 0.01f, 0.01f, 0.01f, -0.01f);
        g_renderer->submitQuad(quad, intersection, Vec4(1, 0, 1, 1));
        g_renderer->submitLine(player->entity.pos, player->entity.pos + dir.normalize() * 0.3f, Vec2(0,0));
    }
}

void Game::drawActor(Actor *e) const
{
    g_renderer->submitSprite(e->sprite, Mat3::translation(e->entity.pos) * Mat3::rotation(TO_RADIANS(e->entity.rotation)), view);
}


Mat3 screenToWorldProjection(Game* game)
{
    Camera* camera = game->currentCamera;
    Mat3 result = Mat3::translation(camera->entity.pos)
        * Mat3::rotation(TO_RADIANS(camera->entity.rotation))
        * Mat3::scale(Vec2(camera->zoom, camera->zoom / g_aspect));
    return result;
}

TileMap* getTileMapAtWorldPos(Vec2 pos, Game* game)
{
    if (!game) return NULL;
    for (size_t i = 0; i < game->tileMaps.size; ++i)
    {
        TileMap* tileMap = &game->tileMaps[i];
        Vec2 tileMapPos = Vec2(tileMap->position.x * TILE_SIZE, tileMap->position.y * TILE_SIZE);
        Vec2 tileMapDim = tileMap->getWorldAbsSize();

        if (vec2WithinRect(pos, tileMapPos, tileMapPos + tileMapDim))
        {
            return tileMap;
        }
            
    }
    return NULL;
}

void getDataFromPos(Game* game, Vec2 pos, DataStrings* result)
{
    
    static Quad entityQuad {-0.1f, -0.1f, -0.1f, 0.1f, 0.1f, 0.1f, 0.1f, -0.1f};
    for (int i = 0; i < game->numActors; ++i)
    {
        Actor* actor = game->actors[i];
        if (vec2WithinRect(pos, actor->entity.pos + entityQuad.point0, actor->entity.pos + entityQuad.point2))
        {
            result->strings.push_back(actor->entity.name);
            result->strings.push_back(actor->sprite->texture->filepath.c_str());
            return; 
        }
                           
    }
    TileMap* tileMap = getTileMapAtWorldPos(pos, game);
    Tile* tile = NULL;
    iPoint tileIndex {0, 0};
    if (tileMap)
    {
        tile = tileMap->getTileAtPos(pos);
        tileIndex = tileMap->getTileIndexAt(pos);
    }
    if (!tileMap || !tile
        || tileIndex.x < 0 || tileIndex.x >= game->tileMaps[0].width
        || tileIndex.y < 0 || tileIndex.y >= game->tileMaps[0].height)
    {
        result->strings.push_back("No data");
        return;
    }
    result->strings.push_back("Tile index: X(" + toString(tileIndex.x) + ") Y("  +
                              toString(tileIndex.y) + ")");
    
    result->strings.push_back("Barrier: " + toString(tile->barrier));
    if (tile->texture)
    {
        result->strings.push_back("Texture: " + String(tile->texture->filepath.c_str()));
    }
}



bool loadTileCollection(const char* filepath, TileCollection* collection)
{
        String tilefile;
        loadTextFile(filepath, &tilefile);
        TextParser parser;
        initParser(&parser, tilefile);

        setSeparator(' ', true, &parser);
        bool tileParsed = false;
        String tileTexLoc(30);
        size_t tilesLoaded = 0;
        size_t tileLoadsFailed = 0;

        Vector<Tile> tiles;
        Vector<String> names;
        tiles.init(20);
        names.init(20);
        bool failed = false;
        const char* tileErrMessage = "Could not load tilecollection: ";
        while (getLine(&parser, &tileTexLoc))
        {
            Texture* tileTex = Texture::loadTexture(tileTexLoc);
            if (!tileTex)
            {
                err("%s%s - Invalid texture\n", tileErrMessage, filepath);
                failed = true;
                break;
            }
            bool tileBarrier;
            String barrier(1);
            bool tileBarrierFound = false;
            if (getLine(&parser, &barrier))
            {
                if (barrier == "0" || barrier == "1")
                {
                    tileBarrier = (barrier != "0");
                    tileBarrierFound = true;
                }                
            }
            if (!tileBarrierFound)
            {
                err("%s%s - Invalid barrier\n", tileErrMessage, filepath);
                failed = true;
                break;
            }
            
            String name(30);
            if (getLine(&parser, &name))
            {
                tiles.push_back({tileBarrier, tileTex});
                names.push_back(name);
                ++tilesLoaded;
            }
            else
            {
                err("%s%s - Invalid name\n", tileErrMessage, filepath);
                failed = true;
                break;
            }            
            
        }
        for (size_t i = 0; i < tiles.size; ++i)
        {
            if (failed)
            {
                tiles[i].texture->del();
            }
            else
            {
                bool result = addTile(tiles[i].texture, tiles[i].barrier, names[i], collection);
                ASSERT(result);
            }
        }
        if (failed)
        {
            return false;
        }
        message("Tile loaded: %d\n", tilesLoaded);
        return true;
}


Vec2 checkCollision(Vec2 pos, Vec2 vel, Game* game)
{
    Vec2 nextPos = pos + vel;
    TileMap* currentTileMap = NULL;
    TileMap* nextTileMap = NULL;

    for (size_t i = 0; i < game->tileMaps.size; ++i)
    {
        TileMap* tileMap = &game->tileMaps[i];
        
        iPoint currentTileIndex = tileMap->getTileIndexAt(pos);
        if (currentTileIndex.x >= 0 && currentTileIndex.x < tileMap->width &&
            currentTileIndex.y >= 0 && currentTileIndex.y < tileMap->height)
        {
            
            currentTileMap = tileMap;
            break;
        }
    }

    
    for (size_t i = 0; i < game->tileMaps.size; ++i)
    {
        TileMap* tileMap = &game->tileMaps[i];
        
        iPoint nextTileIndex = tileMap->getTileIndexAt(nextPos);
        if (nextTileIndex.x >= 0 && nextTileIndex.x < tileMap->width &&
            nextTileIndex.y >= 0 && nextTileIndex.y < tileMap->height)
        {
            
            nextTileMap = tileMap;
            break;
        }
    }

    
    Tile* tile;
    if (!nextTileMap)
    {
        tile = g_voidTile;
    }
    else
    {
        tile = nextTileMap->getTileAtPos(nextPos);
    }
    if (!tile->barrier)
    {
        return vel;
    }
    
    ASSERT(tile && currentTileMap);
    float newXVel;
    float newYVel;
    
    Tile* xTile = currentTileMap->getTileAtPos(Vec2(nextPos.x, pos.y));
    Tile* yTile = currentTileMap->getTileAtPos(Vec2(pos.x, nextPos.y));
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

#include "game.h"
#include "../globals.h"
#include "../defs.h"
#include "../graphics/shader.h"
#include "../graphics/texture.h"
#include "../math/mat3.h"
#include "../math/math.h"
#include "../graphics/renderer.h"

static float speed;
static float accSpeed;


Game::Game()
    : numActors(0)
{
    speed = 1.0f;
    accSpeed = speed * 2;
    player = spawnActor(Vec2(3.0f, 3.25f));
    player->entity.name = "Player";
    //player->entity.rotation = 45.0f;
    camera.entity.pos = player->entity.pos;
    camera.entity.rotation = 0;
    currentCamera = &camera;
    debugCamera.entity.pos = player->entity.pos;
    debugCamera.entity.rotation = 0;
    for (int i = 0; i < 3; ++i)
    {
        spawnActor(Vec2(0.7f + i * 0.4f, 0));
    }
    if (tileMap.init(50, 40))
    {
        for (int y = 0; y < tileMap.height; ++y)
        {
            for (int x = 0; x < tileMap.width; ++x)
            {
                if (y % 5 == 0 && x % 8 == 0)
                {
                    tileMap.setTile(x, y, g_wallTile);
                }
                else
                {
                    tileMap.setTile(x, y, g_thingyTile);
                }
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
        std::cout << "[WARNING] Could not spawn actor - Limit reached!" << std::endl;
        return nullptr;
    }
    static Sprite entitySprite {g_entityTexture, Quad(-0.1f, -0.1f, -0.1f, 0.1f, 0.1f, 0.1f, 0.1f, -0.1f)};
    Actor *e = &actorPool[numActors];
    static unsigned int currentActorID = 0;
    e->entity.id = currentActorID++;
    e->entity.name = "Thingy " + std::to_string(e->entity.id);
    e->entity.pos = pos;
    e->entity.rotation = 0.0f;
    e->sprite = &entitySprite;
    e->speed = speed;
    actors[numActors++] = e;
    return e;
}

void Game::update()
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

    for (int i = 0; i < numActors; ++i)
    {
        Actor *e = actors[i];
        if (e->entity.vel.length() > 0)
        {
            Vec2 newVel = tileMap.checkTileCollision(e->entity.pos, e->entity.vel * g_frameTime);
            e->entity.pos += newVel;
        }
    }

    Tile* currentPlayerTile = tileMap.getTileAtPos(player->entity.pos);    

    //player->entity.rotation += 0.05f;
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
    if (g_debugMode)
    {
        g_renderer->submitText("Player Pos: X - " + std::to_string(player->entity.pos.x) +
                               ", Y - " + std::to_string(player->entity.pos.y), Vec2(-0.1f, -0.3f));
    }                      

}

static Mat3 view;
void Game::render()
{

    view = Mat3::view(Vec2() - currentCamera->entity.pos,
                      TO_RADIANS(-currentCamera->entity.rotation),
                      g_aspect);  
//    view = Mat3::rotation(TO_RADIANS(-currentCamera->entity.rotation)) * Mat3::translation(Vec2() - currentCamera->entity.pos);
    g_renderer->setView(view);
    tileMap.draw(&camera, view);
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
        Vec2 intersection = tileMap.findTileIntersection(player->entity.pos, dir);
        Quad quad = Quad(-0.01f, -0.01f, -0.01f, 0.01f, 0.01f, 0.01f, 0.01f, -0.01f);
        g_renderer->submitQuad(quad, intersection, Vec4(1, 0, 1, 1));
        g_renderer->submitLine(player->entity.pos, player->entity.pos + dir.normalize() * 0.3f, Vec2(0,0));
    }
}

void Game::drawActor(Actor *e) const
{
    /*
    if (g_enableWireframe)
    {
        g_renderer->renderVAO(e->vao, e->texture, Mat3::translation(e->entity.pos) * Mat3::rotation(TO_RADIANS(e->entity.rotation)), view, RENDER_SOLID_AND_WIREFRAME);

    }
    else
    {
    g_renderer->renderVAO(e->vao, e->texture, Mat3::translation(e->entity.pos) * Mat3::rotation(TO_RADIANS(e->entity.rotation)), view, RENDER_SOLID);
    }*/
    g_renderer->submitSprite(e->sprite, Mat3::translation(e->entity.pos) * Mat3::rotation(TO_RADIANS(e->entity.rotation)));
}


Mat3 screenToWorldProjection(Game* game)
{
    Camera* camera = game->currentCamera;
    Mat3 result = Mat3::translation(camera->entity.pos)
        * Mat3::rotation(TO_RADIANS(camera->entity.rotation))
        * Mat3::scale(Vec2(1, 1.0f / g_aspect));
    return result;
}

DataStrings getDataFromPos(Game* game, Vec2 pos)
{
    DataStrings result;
    static Quad entityQuad {-0.1f, -0.1f, -0.1f, 0.1f, 0.1f, 0.1f, 0.1f, -0.1f};
    for (int i = 0; i < game->numActors; ++i)
    {
        Actor* actor = game->actors[i];
        if (vec2IsBetween(pos, actor->entity.pos + entityQuad.point0, actor->entity.pos + entityQuad.point2))
        {
            result.strings.push_back(actor->entity.name);
            result.strings.push_back(actor->sprite->texture->filepath);
            return result; 
        }
                           
    }
    const Tile* tile = game->tileMap.getTileAtPos(pos);
    if (!tile)
    {
        result.strings.push_back("No data");
        return result;
    }
    result.strings.push_back("Barrier: " + std::to_string(tile->barrier));
    if (tile->texture)
    {
        result.strings.push_back("Texture: " + tile->texture->filepath);
    }
    return result;
}

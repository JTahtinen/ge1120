#include "editor.h"
#include "../globals.h"
#include "../defs.h"
#include "../graphics/renderer.h"
#include "../context.h"

static void addSelectableTile(Tile* tile, const String& name, Editor* editor)
{
    if (!tile)
    {
        warn("Could not add tile to editor - pointer was NULL!\n");
        return;
    }
    
    editor->selectableTiles.push_back(tile);
    editor->buttonHandles.push_back(createButton(name, editor->box));
}

bool initEditor(Editor* editor, Game* game, UI* ui)
{
    editor->box = createButtonBox("TILES", ui);
    if (!game)
    {
        err("Could not create editor - Game was NULL!\n");
        return false;
    }
    editor->game = game;
    editor->buttonHandles.init(10);
    editor->selectableTiles.init(10);
    editor->hotTileIndex = {0, 0};
    editor->buttonHandles.push_back(createButton("Empty", editor->box));

    TileCollection& tileCollection = game->tileCollection;
    
    for (size_t i = 0; i < tileCollection.names.size; ++i)
    {
        addSelectableTile(&tileCollection.tiles[i], tileCollection.names[i], editor);
    }
    
    editor->selectedTile = NULL;
    editor->paintMode = false;
    return true;
}

void updateEditor(Editor* editor)
{
    ASSERT(editor->game);
    if (!editor->paintMode)
    {
        //updateUI(editor->box);
    }
    if (doButton(editor->buttonHandles[0], editor->box))
    {
        editor->selectedTile = NULL;
    }
    for (unsigned int i = 1; i < editor->buttonHandles.size; ++i)
    {
        if (doButton(editor->buttonHandles[i], editor->box))
        {
            editor->selectedTile = editor->selectableTiles[i - 1];
        }
    }
    if (g_screenContext == CONTEXT_GAME || g_screenContext == CONTEXT_EVERYTHING)// && !editor->box->inFocus && !editor->box->drag)
    {
        editor->lastHotTileIndex = editor->hotTileIndex;
        //TODO: Check which tilemap the cursor is on
        TileMap* tileMap = getTileMapAtWorldPos(g_projectedMousePos, editor->game);
        if (!tileMap) return;
        editor->hotTileIndex = tileMap->getTileIndexAt(g_projectedMousePos);
        bool setTile;
        if (editor->selectedTile && (g_input.mouseLeftClicked ||
                                     (g_input.mouseLeftHeld && editor->hotTileIndex != editor->lastHotTileIndex)))
        {
            //if (!(g_input.mouseLeftClicked && isCursorOnBox(editor->box)))
            //{
            setTile = true;
            editor->paintMode = true;
            //}
        }
        else
        {
            setTile = false;
        }
        if (g_input.mouseLeftReleased)
        {
            editor->paintMode = false;
        }
        if (setTile)
        {            
            tileMap->setTile(editor->hotTileIndex.x, editor->hotTileIndex.y,
                             editor->selectedTile);
        }
    }
//    g_screenContext = (editor->paintMode) ? CONTEXT_GAME : CONTEXT_EVERYTHING;
}

void viewEditor(Editor* editor)
{
    static Quad spriteBGQuad(0, 0, 0, 0.2f, 0.2f, 0.2f, 0.2f, 0);
    static Quad spriteQuad(0.02f, 0.02f, 0.02, 0.18f, 0.18f, 0.18f, 0.18f, 0.02);
    static Sprite tileSprite { NULL, spriteQuad };
    
    if (editor->selectedTile)
    {
        tileSprite.texture = editor->selectedTile->texture;
    }
    else
    {
        tileSprite.texture = NULL;
    }
    g_uiRenderer->setView(g_view);
    g_uiRenderer->submitQuad(spriteBGQuad, Vec2(0.7, 0.3f), Vec4(0, 0, 0, 1));
    if (tileSprite.texture)
    {
        g_uiRenderer->submitSprite(&tileSprite,
                                   Mat3::translation(Vec2(0.7f, 0.3f)), g_view);
    }                            
    //drawUI(&editor->box);        
}

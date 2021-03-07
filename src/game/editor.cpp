#include "editor.h"
#include "../globals.h"
#include "../defs.h"
#include "../graphics/renderer.h"

static void addSelectableTile(Tile* tile, std::string name, Editor* editor)
{
    if (!tile)
    {
        warn("Could not add tile to editor - pointer was NULL!\n");
        return;
    }
    
    editor->selectableTiles.push_back(tile);
    editor->buttonHandles.push_back(createButton(name, &editor->ui));
}

bool initEditor(Editor* editor, Game* game)
{
    if (!initUI("EDITOR", &editor->ui))
    {
        err("Could not init editor - UI Creation failed!\n");
        return false;
    }
    if (!game)
    {
        err("Could not create editor - Game was NULL!\n");
        return false;
    }
    editor->game = game;
    editor->buttonHandles.init(10);
    editor->selectableTiles.init(10);
    editor->hotTileIndex = {0, 0};
    addSelectableTile(g_thingyTile, "Grass tile", editor);
    addSelectableTile(g_wallTile, "Wall tile", editor);
    
    editor->selectedTile = NULL;
    return true;
}

void updateEditor(Editor* editor)
{
    ASSERT(editor->game);
    updateUI(&editor->ui);
    for (unsigned int i = 0; i < editor->buttonHandles.size; ++i)
    {
        if (doButton(editor->buttonHandles[i], &editor->ui))
        {
            editor->selectedTile = editor->selectableTiles[i];
        }
    }
    if (!editor->ui.inFocus && !editor->ui.drag)
    {
        editor->lastHotTileIndex = editor->hotTileIndex;
        editor->hotTileIndex = editor->game->tileMap.getTileIndexAt(g_projectedMousePos);
        bool setTile;
        if (g_input.mouseLeftClicked)
            setTile = true;
        else
        if (g_input.mouseLeftHeld && editor->hotTileIndex != editor->lastHotTileIndex)
            setTile = true;
        else
            setTile = false;
        {
            if (setTile && editor->selectedTile)
            {            
                editor->game->tileMap.setTile(editor->hotTileIndex.x, editor->hotTileIndex.y,
                                              editor->selectedTile);
            }
        }
    }

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
    //g_uiRenderer->submitQuad(spriteBGQuad, Vec2(0.7, 0.3f), Vec4(0, 0, 0, 1));
    if (tileSprite.texture)
    {
        g_uiRenderer->submitSprite(&tileSprite, Mat3::translation(Vec2(0.7f, 0.3f)), g_view);
    }                            
        drawUI(&editor->ui);    
}

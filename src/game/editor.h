#pragma once
#include "../ui/ui.h"
#include "tilemap.h"
#include "../util/vector.h"
#include "game.h"
#include "../math/math.h"
struct Editor
{
    Game* game;
    UI ui;
    Vector<unsigned int> buttonHandles;
    Vector<Tile*> selectableTiles;
    Tile* selectedTile;
    iPoint lastHotTileIndex;
    iPoint hotTileIndex;
    bool paintMode;
};

bool initEditor(Editor* editor, Game* game);
void updateEditor(Editor* editor);
void viewEditor(Editor* editor);

#pragma once
#include "../util/vector.h"
#include "../util/string.h"
#include "../graphics/texture.h"

struct Tile
{
    bool barrier;
    Texture* texture;
};

struct TileCollection
{
    Vector<Tile> tiles;
    Vector<String> names;
};


Tile* getTile(const String& name, TileCollection* collection);
//bool addTile(Tile* tile, const String& name, TileCollection* collection);
bool addTile(Texture* texture, bool barrier, const String& name, TileCollection* collection);

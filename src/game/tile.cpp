#include "tile.h"

Tile* getTile(const String& name, TileCollection* collection)
{
    if (!collection)
    {
        err("Could not get tile - Collection was NULL!\n");
        return NULL;
    }
    ASSERT(collection->tiles.size == collection->names.size);
    Tile* result = NULL;
    
    for (size_t i = 0; i < collection->tiles.size; ++i)
    {
        if (collection->names[i] == name)
        {
            result = collection->tiles[i];
            break;
        }
    }
    if (!result)
    {
        warn("Could not find texture: %s from tile collection\n", name.c_str());
    }
    return result;
}



bool addTile(Tile* tile, const char* name, TileCollection* collection)
{
    if (!tile)
    {
        err("Could not add tile to collection - Tile was NULL!\n");
        return false;
    }
    if (!collection)
    {
        err("Could not add tile to collection - Collection was NULL!\n");
        return false;
    }
   


    collection->tiles.push_back(tile);
    collection->names.push_back(name);
    return true;    
}



#pragma once
#include "game/maploader.h"
#include "game/levelmetrics.h"

struct Level
{
    MapLoader map;
    LevelMetrics metrics;
};

bool loadLevel(Level &lvl, const char *mapPath, float tileSize);
void applySpawn(const Level &lvl, float &camX, float &camZ);

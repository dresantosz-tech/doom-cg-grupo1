#pragma once

#include "core/game_enums.h"
#include "level/level.h"

Level &gameLevel();

// Estado do jogo
GameState gameGetState();
void gameSetState(GameState s);
void gameTogglePause();

// Acesso controlado ao Level (se alguém ainda precisar)
Level &gameLevel();

// Fluxo principal
bool gameInit(const char *mapPath);
void gameUpdate(float dt);
void gameRender();
void gameReset();

// Ações do player
void playerTryAttack();
void playerTryReload();
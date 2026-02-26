// #include <GL/glut.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "core/game_enums.h"
#include "core/game_state.h"

#include "core/game.h"

#include "level/level.h"

#include "core/camera.h"
#include "input/input.h"
#include "input/keystate.h"

#include "graphics/drawlevel.h"
#include "graphics/skybox.h"
#include "graphics/hud.h"
#include "graphics/menu.h"
#include "graphics/lighting.h"

#include "core/movement.h"
#include "core/entities.h"

#include "audio/audio_system.h"

#include "utils/assets.h"
#include "core/config.h"

#include "core/window.h"

#include <GL/glew.h>
#include <GL/glut.h>

static HudTextures gHudTex;
static GameContext g;
static constexpr int kTotalMaps = 3;
static const char *kMapSequence[kTotalMaps] = {
    //"maps/mapTestes.txt",
    "maps/map1.txt",
    "maps/map2.txt",
    "maps/map3.txt"};
static int gCurrentMapIndex = 0;

// --- Assets / Level ---
static GameAssets gAssets;
Level gLevel;
static AudioSystem gAudioSys;

GameContext &gameContext() { return g; }

AudioSystem &gameAudio() { return gAudioSys; }

Level &gameLevel() { return gLevel; }

GameState gameGetState() { return g.state; }

void gameSetState(GameState s) { g.state = s; }

void gameTogglePause()
{
    if (g.state == GameState::JOGANDO)
        g.state = GameState::PAUSADO;
    else if (g.state == GameState::PAUSADO)
        g.state = GameState::JOGANDO;
}

static bool loadCurrentMapAndSpawn()
{
    if (gCurrentMapIndex < 0 || gCurrentMapIndex >= kTotalMaps)
        return false;

    if (!loadLevel(gLevel, kMapSequence[gCurrentMapIndex], GameConfig::TILE_SIZE))
        return false;

    g.r.mapTheme = gCurrentMapIndex + 1;
    if (gCurrentMapIndex == 0)
        g.r.texSkydome = gAssets.texSkydome;
    else if (gCurrentMapIndex == 1)
        g.r.texSkydome = (gAssets.texSkydome2 != 0) ? gAssets.texSkydome2 : gAssets.texSkydome;
    else if (gCurrentMapIndex == 2)
        g.r.texSkydome = (gAssets.texSkydome3 != 0) ? gAssets.texSkydome3 : gAssets.texSkydome;

    applySpawn(gLevel, camX, camZ);
    camY = GameConfig::PLAYER_EYE_Y;
    return true;
}

static bool playerOnMapTile(char tileId)
{
    const auto &data = gLevel.map.data();
    if (data.empty())
        return false;

    const float tile = gLevel.metrics.tile;
    const float localX = camX - gLevel.metrics.offsetX;
    const float localZ = camZ - gLevel.metrics.offsetZ;

    int tx = (int)std::floor(localX / tile);
    int tz = (int)std::floor(localZ / tile);

    if (tz < 0 || tz >= (int)data.size())
        return false;
    if (tx < 0 || tx >= (int)data[tz].size())
        return false;

    return data[tz][tx] == tileId;
}

static bool playerNearKeyForPickupPrompt()
{
    if (g.player.hasKey)
        return false;

    const auto &items = gLevel.items;
    for (const auto &item : items)
    {
        if (!item.active || item.type != ITEM_KEY)
            continue;

        float dx = camX - item.x;
        float dz = camZ - item.z;
        if (dx * dx + dz * dz <= 2.25f)
            return true;
    }
    return false;
}

static bool playerNearDoorForPrompt()
{
    const auto &data = gLevel.map.data();
    if (data.empty())
        return false;

    const float tile = gLevel.metrics.tile;
    const float offX = gLevel.metrics.offsetX;
    const float offZ = gLevel.metrics.offsetZ;
    const int tx0 = (int)((camX - offX) / tile);
    const int tz0 = (int)((camZ - offZ) / tile);

    for (int dz = -1; dz <= 1; ++dz)
    {
        for (int dx = -1; dx <= 1; ++dx)
        {
            const int tx = tx0 + dx;
            const int tz = tz0 + dz;
            if (tz < 0 || tz >= (int)data.size())
                continue;
            if (tx < 0 || tx >= (int)data[tz].size())
                continue;
            if (data[tz][tx] != 'D')
                continue;

            float wx, wz;
            gLevel.metrics.tileCenter(tx, tz, wx, wz);
            float half = tile * 0.5f;
            float minX = wx - half;
            float maxX = wx + half;
            float minZ = wz - half;
            float maxZ = wz + half;

            float cx = std::fmax(minX, std::fmin(camX, maxX));
            float cz = std::fmax(minZ, std::fmin(camZ, maxZ));
            float ddx = camX - cx;
            float ddz = camZ - cz;
            if (ddx * ddx + ddz * ddz <= (1.8f * 1.8f))
                return true;
        }
    }
    return false;
}

// --- INIT ---
bool gameInit(const char *mapPath)
{
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);

    setupSunLightOnce();
    setupIndoorLightOnce();

    if (!loadAssets(gAssets))
        return false;

    g.r.texChao1 = gAssets.texChao1;
    g.r.texChao2 = gAssets.texChao2;
    g.r.texChao3 = gAssets.texChao3;
    g.r.texChao4 = gAssets.texChao4;
    g.r.texParede1 = gAssets.texParede1;
    g.r.texParede2 = gAssets.texParede2;
    g.r.texParede3 = gAssets.texParede3;
    g.r.texPorta = gAssets.texPorta;
    g.r.texSangue = gAssets.texSangue;
    g.r.texLava = gAssets.texLava;
    g.r.texChaoInterno = gAssets.texChaoInterno;
    g.r.texParedeInterna = gAssets.texParedeInterna;
    g.r.texTeto = gAssets.texTeto;

    g.r.texSkydome = gAssets.texSkydome;
    g.r.texSkydome2 = gAssets.texSkydome2;
    g.r.texSkydome3 = gAssets.texSkydome3;
    g.r.texMenuBG = gAssets.texMenuBG;
    g.r.texEndBG = gAssets.texEndBG;

    gHudTex.texHudFundo = gAssets.texHudFundo;
    gHudTex.texHealthHudIcon = gAssets.texHpHUD;
    gHudTex.texStaminaHudIcon = gAssets.texStaminaHUD;
    gHudTex.texKeyIcon = gAssets.texKey;
    gHudTex.texNoKeyIcon = gAssets.texHandHUD;
    gHudTex.texGunHUD = gAssets.texGunHUD;

    gHudTex.texGunDefault = gAssets.texGunDefault;
    gHudTex.texGunFire1 = gAssets.texGunFire1;
    gHudTex.texGunFire2 = gAssets.texGunFire2;
    gHudTex.texGunReload1 = gAssets.texGunReload1;
    gHudTex.texGunReload2 = gAssets.texGunReload2;

    gHudTex.texDamage = gAssets.texDamage;
    gHudTex.texHealthOverlay = gAssets.texHealthOverlay;

    for (int i = 0; i < 5; i++)
    {
        g.r.texEnemies[i] = gAssets.texEnemies[i];
        g.r.texEnemiesRage[i] = gAssets.texEnemiesRage[i];
        g.r.texEnemiesDamage[i] = gAssets.texEnemiesDamage[i];
    }

    g.r.texHealth = gAssets.texHealth;
    g.r.texAmmo = gAssets.texKey;

    g.r.progSangue = gAssets.progSangue;
    g.r.progLava = gAssets.progLava;
    g.r.progExit = gAssets.progExit;

    (void)mapPath;
    gCurrentMapIndex = 0;
    if (!loadCurrentMapAndSpawn())
        return false;

    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutSpecialFunc(specialDown);
    glutSpecialUpFunc(specialUp);
    glutPassiveMotionFunc(mouseMotion);
    glutSetCursor(GLUT_CURSOR_NONE);

    // Audio init + ambient + enemy sources
    audioInit(gAudioSys, gLevel);

    g.state = GameState::MENU_INICIAL;
    g.time = 0.0f;
    g.player = PlayerState{};

    return true;
}

// Reinicia o jogo
void gameReset()
{
    gCurrentMapIndex = 0;
    loadCurrentMapAndSpawn();

    g.player.health = 100;
    g.player.stamina = 100.0f;
    g.player.hasKey = false;

    g.player.damageAlpha = 0.0f;
    g.player.healthAlpha = 0.0f;

    // limpa input residual ao reset
    keyW = keyA = keyS = keyD = false;
    keyShift = false;
}

void gameUpdate(float dt)
{
    g.time += dt;

    // 1. SE NÃO ESTIVER JOGANDO, NÃO RODA A LÓGICA DO JOGO
    if (g.state != GameState::JOGANDO)
    {
        return;
    }

    atualizaMovimento(dt);

    AudioListener L;
    L.pos = {camX, camY, camZ};
    {
        float ry = yaw * 3.14159f / 180.0f;
        float rp = pitch * 3.14159f / 180.0f;
        L.forward = {cosf(rp) * sinf(ry), sinf(rp), -cosf(rp) * cosf(ry)};
    }
    L.up = {0.0f, 1.0f, 0.0f};
    L.vel = {0.0f, 0.0f, 0.0f};

    bool moving = (keyW || keyA || keyS || keyD);
    audioUpdate(gAudioSys, gLevel, L, dt, moving, g.player.health);

    if (g.player.damageAlpha > 0.0f)
    {
        g.player.damageAlpha -= dt * 0.5f;
        if (g.player.damageAlpha < 0.0f)
            g.player.damageAlpha = 0.0f;
    }
    if (g.player.healthAlpha > 0.0f)
    {
        g.player.healthAlpha -= dt * 0.9f;
        if (g.player.healthAlpha < 0.0f)
            g.player.healthAlpha = 0.0f;
    }

    updateEntities(dt);

    // Progresso de mapa via tile 'S'
    if (playerOnMapTile('S'))
    {
        if (gCurrentMapIndex + 1 < kTotalMaps)
        {
            gCurrentMapIndex++;
            if (!loadCurrentMapAndSpawn())
            {
                g.state = GameState::GAME_OVER;
                g.player.damageAlpha = 1.0f;
                return;
            }
        }
        else
        {
            g.state = GameState::VITORIA;
            return;
        }
    }

    // 3. CHECAGEM DE GAME OVER
    if (g.player.health <= 0)
    {
        g.state = GameState::GAME_OVER;
        g.player.damageAlpha = 1.0f;
    }
}

// Função auxiliar para desenhar o mundo 3D (Inimigos, Mapa, Céu)
void drawWorld3D()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // LIGAR O 3D
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);

    // Configuração da Câmera
    float radYaw = yaw * 3.14159265f / 180.0f;
    float radPitch = pitch * 3.14159265f / 180.0f;
    float dirX = cosf(radPitch) * sinf(radYaw);
    float dirY = sinf(radPitch);
    float dirZ = -cosf(radPitch) * cosf(radYaw);
    gluLookAt(camX, camY, camZ, camX + dirX, camY + dirY, camZ + dirZ, 0.0f, 1.0f, 0.0f);

    // Desenha o cenário
    setSunDirectionEachFrame();
    drawSkydome(camX, camY, camZ, g.r);
    drawLevel(gLevel.map, camX, camZ, dirX, dirZ, g.r, g.time);
    drawEntities(gLevel.enemies, gLevel.items, camX, camZ, dirX, dirZ, g.r);
}

// FUNÇÃO PRINCIPAL DE DESENHO (REFATORADA: usa menuRender / pauseMenuRender / hudRenderAll)
void gameRender()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Monta o estado do HUD a partir das variáveis globais do jogo
    HudState hs;
    hs.playerHealth = g.player.health;
    hs.playerStamina = (int)g.player.stamina;
    hs.hasKey = g.player.hasKey;
    hs.showKeyPickupPrompt = playerNearKeyForPickupPrompt();
    hs.showDoorPrompt = playerNearDoorForPrompt();
    hs.canUnlockDoor = hs.showDoorPrompt && g.player.hasKey;
    hs.isMoving = (keyW || keyA || keyS || keyD);
    hs.isSprinting = hs.isMoving && keyShift && (g.player.stamina > 0.0f);
    hs.gameTime = g.time;
    hs.damageAlpha = g.player.damageAlpha;
    hs.healthAlpha = g.player.healthAlpha;

    // --- ESTADO: MENU INICIAL ---
    if (g.state == GameState::MENU_INICIAL)
    {
        // menuRender já cuida do fogo (update + render)
        menuRender(janelaW, janelaH, g.time, "", "Pressione ENTER para Jogar", g.r);
    }
    // --- ESTADO: GAME OVER ---
    else if (g.state == GameState::GAME_OVER)
    {
        // Fundo 3D congelado
        drawWorld3D();

        // OVERLAY DO MELT por cima do jogo
        // menuMeltRenderOverlay(janelaW, janelaH, g.time);

        // Tela do game over por cima (com fogo)
        menuRender(janelaW, janelaH, g.time, "GAME OVER", "Pressione ENTER para Reiniciar", g.r);
    }
    // --- ESTADO: VITORIA ---
    else if (g.state == GameState::VITORIA)
    {
        drawWorld3D();
        RenderAssets endScreen = g.r;
        endScreen.texMenuBG = g.r.texEndBG;
        menuRender(janelaW, janelaH, g.time, "VITORIA", "Pressione ENTER para Reiniciar", endScreen);
    }
    // --- ESTADO: PAUSADO ---
    else if (g.state == GameState::PAUSADO)
    {
        // 1) Mundo 3D congelado
        drawWorld3D();

        // 2) HUD normal (arma + barra + mira + overlays)
        hudRenderAll(janelaW, janelaH, gHudTex, hs, true, true, true);

        // 3) Menu escuro por cima
        pauseMenuRender(janelaW, janelaH, g.time);
    }
    // --- ESTADO: JOGANDO ---
    else // JOGANDO
    {
        // 1) Mundo 3D
        drawWorld3D();

        // 2) HUD completo
        hudRenderAll(janelaW, janelaH, gHudTex, hs, true, true, true);

        menuMeltRenderOverlay(janelaW, janelaH, g.time);
    }

    glutSwapBuffers();
}


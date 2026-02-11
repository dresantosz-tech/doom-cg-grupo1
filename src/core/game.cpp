#include "core/game_enums.h"
#include "core/game_state.h"
#include "audio/audio_system.h"
#include "graphics/hud.h"
#include "graphics/menu.h"
#include "graphics/fire_particles.h"
#include "core/game.h"
#include "utils/assets.h"
#include "level/level.h"
#include "core/config.h"
#include "graphics/skybox.h"
#include "input/keystate.h"
#include "core/camera.h"
#include "input/input.h"
#include "graphics/drawlevel.h"
#include "core/movement.h"
#include "core/entities.h"
#include <GL/glut.h>
#include "core/window.h"
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>
#include <cstdlib>

static FireSystem gFire;
static HudTextures gHudTex;
static GameContext g;

constexpr int MAX_MAGAZINE = 12;

// --- Assets / Level ---
static GameAssets gAssets;
Level gLevel;

// Configurações da IA
const float ENEMY_SPEED = 2.5f;
const float ENEMY_VIEW_DIST = 15.0f;
const float ENEMY_ATTACK_DIST = 1.5f;

static AudioSystem gAudioSys;

GameState gameGetState() { return g.state; }
void gameSetState(GameState s) { g.state = s; }

void gameTogglePause()
{
    if (g.state == GameState::JOGANDO)
        g.state = GameState::PAUSADO;
    else if (g.state == GameState::PAUSADO)
        g.state = GameState::JOGANDO;
}

Level &gameLevel() { return gLevel; } 

// --- FUNÇÕES AUXILIARES DE LUZ ---
static void setupIndoorLightOnce()
{
    glEnable(GL_LIGHT1);
    GLfloat lampDiffuse[] = {1.7f, 1.7f, 1.8f, 1.0f};
    GLfloat lampSpecular[] = {0, 0, 0, 1.0f};
    GLfloat lampAmbient[] = {0.98f, 0.99f, 1.41f, 1.0f};
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lampDiffuse);
    glLightfv(GL_LIGHT1, GL_SPECULAR, lampSpecular);
    glLightfv(GL_LIGHT1, GL_AMBIENT, lampAmbient);
    glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.6f);
    glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, 0.06f);
    glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.02f);
    glDisable(GL_LIGHT1);
}

static void setupSunLightOnce()
{
    glEnable(GL_LIGHT0);
    GLfloat sceneAmbient[] = {0.45f, 0.30f, 0.25f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, sceneAmbient);
    GLfloat sunDiffuse[] = {1.4f, 0.55f, 0.30f, 1.0f};
    GLfloat sunSpecular[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sunDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, sunSpecular);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
}

static void setSunDirectionEachFrame()
{
    GLfloat sunDir[] = {0.3f, 1.0f, 0.2f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, sunDir);
}

void playerTryReload()
{
    if (g.weapon.state != WeaponState::W_IDLE)
        return;
    if (g.player.currentAmmo >= MAX_MAGAZINE)
        return;
    if (g.player.reserveAmmo <= 0)
        return;

    g.weapon.state = WeaponState::W_RELOAD_1;
    g.weapon.timer = 0.50f;

    audioPlayReload(gAudioSys);
}

// --- ATAQUE ---
void playerTryAttack()
{
    if (g.weapon.state != WeaponState::W_IDLE)
        return;
    if (g.player.currentAmmo <= 0)
        return;

    g.player.currentAmmo--;

    audioOnPlayerShot(gAudioSys);
    audioPlayShot(gAudioSys);

    g.weapon.state = WeaponState::W_FIRE_1;
    g.weapon.timer = 0.08f;

    for (auto &en : gLevel.enemies)
    {
        if (en.state == STATE_DEAD)
            continue;

        float dx = en.x - camX;
        float dz = en.z - camZ;
        float dist = std::sqrt(dx * dx + dz * dz);

        if (dist < 17.0f)
        {
            float radYaw = yaw * 3.14159f / 180.0f;
            float camDirX = std::sin(radYaw);
            float camDirZ = -std::cos(radYaw);

            float toEnemyX = dx / dist;
            float toEnemyZ = dz / dist;

            float dot = camDirX * toEnemyX + camDirZ * toEnemyZ;

            if (dot > 0.6f)
            {
                en.hp -= 30;
                en.hurtTimer = 0.5f;

                if (en.hp <= 0)
                {
                    en.state = STATE_DEAD;
                    en.respawnTimer = 15.0f;

                    Item drop;
                    drop.type = ITEM_AMMO;
                    drop.x = en.x;
                    drop.z = en.z;
                    drop.active = true;
                    drop.respawnTimer = 0.0f;

                    gLevel.items.push_back(drop);
                }
                return;
            }
        }
    }
}

// --- Walkable ---
bool isWalkable(float x, float z)
{
    float tile = gLevel.metrics.tile;
    float offX = gLevel.metrics.offsetX;
    float offZ = gLevel.metrics.offsetZ;

    int tx = (int)((x - offX) / tile);
    int tz = (int)((z - offZ) / tile);

    const auto &data = gLevel.map.data();

    if (tz < 0 || tz >= (int)data.size())
        return false;
    if (tx < 0 || tx >= (int)data[tz].size())
        return false;

    char c = data[tz][tx];
    if (c == '1' || c == '2')
        return false;

    return true;
}

// --- ENTIDADES ---
void updateEntities(float dt)
{
    // Inimigos
    for (auto &en : gLevel.enemies)
    {
        if (en.state == STATE_DEAD)
        {
            en.respawnTimer -= dt;
            if (en.respawnTimer <= 0.0f)
            {
                en.state = STATE_IDLE;
                en.hp = 100;
                en.x = en.startX;
                en.z = en.startZ;
                en.hurtTimer = 0.0f;
            }
            continue;
        }

        if (en.hurtTimer > 0.0f)
            en.hurtTimer -= dt;

        float dx = camX - en.x;
        float dz = camZ - en.z;
        float dist = std::sqrt(dx * dx + dz * dz);

        switch (en.state)
        {
        case STATE_IDLE:
            if (dist < ENEMY_VIEW_DIST)
                en.state = STATE_CHASE;
            break;

        case STATE_CHASE:
            if (dist < ENEMY_ATTACK_DIST)
            {
                en.state = STATE_ATTACK;
                en.attackCooldown = 0.5f;
            }
            else if (dist > ENEMY_VIEW_DIST * 1.5f)
            {
                en.state = STATE_IDLE;
            }
            else
            {
                float dirX = dx / dist;
                float dirZ = dz / dist;

                float moveStep = ENEMY_SPEED * dt;

                float nextX = en.x + dirX * moveStep;
                if (isWalkable(nextX, en.z))
                    en.x = nextX;

                float nextZ = en.z + dirZ * moveStep;
                if (isWalkable(en.x, nextZ))
                    en.z = nextZ;
            }
            break;

        case STATE_ATTACK:
            if (dist > ENEMY_ATTACK_DIST)
            {
                en.state = STATE_CHASE;
            }
            else
            {
                en.attackCooldown -= dt;
                if (en.attackCooldown <= 0.0f)
                {
                    g.player.health -= 10;
                    en.attackCooldown = 1.0f;
                    g.player.damageAlpha = 1.0f;

                    audioPlayHurt(gAudioSys);
                }
            }
            break;

        default:
            break;
        }
    }

    // Itens
    for (auto &item : gLevel.items)
    {
        if (!item.active)
        {
            item.respawnTimer -= dt;
            if (item.respawnTimer <= 0.0f)
            {
                item.active = true;
            }
            continue;
        }

        float dx = camX - item.x;
        float dz = camZ - item.z;

        if (dx * dx + dz * dz < 1.0f)
        {
            item.active = false;

            if (item.type == ITEM_HEALTH)
            {
                item.respawnTimer = 15.0f;
                g.player.health += 50;
                if (g.player.health > 100)
                    g.player.health = 100;
                g.player.healthAlpha = 1.0f;
            }
            else if (item.type == ITEM_AMMO)
            {
                item.respawnTimer = 999999.0f;
                g.player.reserveAmmo = 20;
            }
        }
    }
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

    g.r.texChao = gAssets.texChao;
    g.r.texParede = gAssets.texParede;
    g.r.texSangue = gAssets.texSangue;
    g.r.texLava = gAssets.texLava;
    g.r.texChaoInterno = gAssets.texChaoInterno;
    g.r.texParedeInterna = gAssets.texParedeInterna;
    g.r.texTeto = gAssets.texTeto;

    g.r.texSkydome = gAssets.texSkydome;

    gHudTex.texHudFundo = gAssets.texHudFundo;
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
    g.r.texAmmo = gAssets.texAmmo;

    g.r.progSangue = gAssets.progSangue;
    g.r.progLava = gAssets.progLava;

    if (!loadLevel(gLevel, mapPath, GameConfig::TILE_SIZE))
        return false;

    applySpawn(gLevel, camX, camZ);
    camY = GameConfig::PLAYER_EYE_Y;

    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyboardUp);
    glutPassiveMotionFunc(mouseMotion);
    glutSetCursor(GLUT_CURSOR_NONE);

    // Audio init + ambient + enemy sources
    audioInit(gAudioSys, gLevel);

    g.state = GameState::MENU_INICIAL;
    g.time = 0.0f;
    g.player = PlayerState{};
    g.weapon = WeaponAnim{};

    return true;
}

// --- WEAPON ANIM ---
void updateWeaponAnim(float dt)
{
    const float TIME_FRAME_2 = 0.12f;
    const float RELOAD_T2 = 0.85f;
    const float RELOAD_T3 = 0.25f;

    if (g.weapon.state == WeaponState::W_IDLE)
        return;

    g.weapon.timer -= dt;
    if (g.weapon.timer > 0.0f)
        return;

    if (g.weapon.state == WeaponState::W_FIRE_1)
    {
        g.weapon.state = WeaponState::W_FIRE_2;
        g.weapon.timer = TIME_FRAME_2;
    }
    else if (g.weapon.state == WeaponState::W_FIRE_2)
    {
        g.weapon.state = WeaponState::W_PUMP;
        g.weapon.timer = AudioTuning::PUMP_TIME;
        audioPlayPumpClick(gAudioSys);
    }
    else if (g.weapon.state == WeaponState::W_RETURN)
    {
        g.weapon.state = WeaponState::W_IDLE;
        g.weapon.timer = 0.0f;
    }
    else if (g.weapon.state == WeaponState::W_PUMP)
    {
        g.weapon.state = WeaponState::W_IDLE;
        g.weapon.timer = 0.0f;
    }
    else if (g.weapon.state == WeaponState::W_RELOAD_1)
    {
        g.weapon.state = WeaponState::W_RELOAD_2;
        g.weapon.timer = RELOAD_T2;
    }
    else if (g.weapon.state == WeaponState::W_RELOAD_2)
    {
        g.weapon.state = WeaponState::W_RELOAD_3;
        g.weapon.timer = RELOAD_T3;
    }
    else if (g.weapon.state == WeaponState::W_RELOAD_3)
    {
        g.weapon.state = WeaponState::W_IDLE;
        g.weapon.timer = 0.0f;

        int needed = MAX_MAGAZINE - g.player.currentAmmo;
        if (needed > g.player.reserveAmmo)
            needed = g.player.reserveAmmo;

        g.player.currentAmmo += needed;
        g.player.reserveAmmo -= needed;
    }
}

// --- Texto ---
void drawText(float x, float y, const char *text, float escala)
{
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(escala, escala, 1.0f);
    glLineWidth(2.0f);

    for (const char *c = text; *c != '\0'; c++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);
    }

    glPopMatrix();
}

// Reinicia o jogo
void gameReset()
{
    g.player.health = 100;
    g.player.currentAmmo = 12;
    g.player.reserveAmmo = 25;

    g.player.damageAlpha = 0.0f;
    g.player.healthAlpha = 0.0f;

    g.weapon.state = WeaponState::W_IDLE;
    g.weapon.timer = 0.0f;
    // Respawna o jogador
    applySpawn(gLevel, camX, camZ);
}

void gameUpdate(float dt)
{
    g.time += dt;

    // 1. SE NÃO ESTIVER JOGANDO, NÃO RODA A LÓGICA DO JOGO
    if (g.state != GameState::JOGANDO)
    {
        return;
    }

    atualizaMovimento();

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
    updateWeaponAnim(dt);

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

// --- MENU DE PAUSE (CENTRALIZADO) ---
void drawPauseMenu()
{
    glPushAttrib(GL_ALL_ATTRIB_BITS);

    // Configura 2D
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_FOG);
    glDisable(GL_CULL_FACE);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, janelaW, 0, janelaH);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // 1. FILTRO ESCURO TRANSPARENTE
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.6f); // 60% Preto
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(janelaW, 0);
    glVertex2f(janelaW, janelaH);
    glVertex2f(0, janelaH);
    glEnd();
    glDisable(GL_BLEND);

    // 2. TÍTULO "PAUSADO"
    std::string txt = "PAUSADO";
    float scale = 0.6f;
    glLineWidth(5.0f); // Borda grossa

    // --- CÁLCULO EXATO DA LARGURA ---
    float wTotal = 0;
    for (char c : txt)
        wTotal += glutStrokeWidth(GLUT_STROKE_ROMAN, c);
    wTotal *= scale; // Ajusta pela escala

    float x = (janelaW - wTotal) / 2.0f;
    float y = (janelaH / 2.0f) + 20.0f;

    // Sombra Preta
    glColor3f(0.0f, 0.0f, 0.0f);
    glPushMatrix();
    glTranslatef(x + 3, y - 3, 0);
    glScalef(scale, scale, 1);
    for (char c : txt)
        glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
    glPopMatrix();

    // Texto Branco
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(scale, scale, 1);
    for (char c : txt)
        glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
    glPopMatrix();

    // 3. SUBTÍTULO
    std::string sub = "Pressione P para Voltar";
    float scaleSub = 0.22f;
    glLineWidth(3.0f);

    // Cálculo Exato Subtítulo
    float wSub = 0;
    for (char c : sub)
        wSub += glutStrokeWidth(GLUT_STROKE_ROMAN, c);
    wSub *= scaleSub;

    float xSub = (janelaW - wSub) / 2.0f;
    float ySub = (janelaH / 2.0f) - 60.0f;

    // Pisca Amarelo
    if ((int)(g.time * 3) % 2 == 0)
        glColor3f(1, 1, 0);
    else
        glColor3f(1, 1, 1);

    glPushMatrix();
    glTranslatef(xSub, ySub, 0);
    glScalef(scaleSub, scaleSub, 1);
    for (char c : sub)
        glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
    glPopMatrix();

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopAttrib();
}

// FUNÇÃO PRINCIPAL DE DESENHO (REFATORADA: usa menuRender / pauseMenuRender / hudRenderAll)
void gameRender()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Monta o estado do HUD a partir das variáveis globais do jogo
    HudState hs;
    hs.playerHealth = g.player.health;
    hs.currentAmmo = g.player.currentAmmo;
    hs.reserveAmmo = g.player.reserveAmmo;
    hs.damageAlpha = g.player.damageAlpha;
    hs.healthAlpha = g.player.healthAlpha;
    hs.weaponState = g.weapon.state;

    // --- ESTADO: MENU INICIAL ---
    if (g.state == GameState::MENU_INICIAL)
    {
        // menuRender já cuida do fogo (update + render)
        menuRender(janelaW, janelaH, g.time, gFire, "DOOM LIKE", "Pressione ENTER para Jogar");
    }
    // --- ESTADO: GAME OVER ---
    else if (g.state == GameState::GAME_OVER)
    {
        // Fundo 3D congelado
        drawWorld3D();

        // HUD mínimo no game over: só barra + overlays (sem arma e sem mira)
        hudRenderAll(janelaW, janelaH, gHudTex, hs, false, false, true);

        // Tela do game over por cima (com fogo)
        menuRender(janelaW, janelaH, g.time, gFire, "GAME OVER", "Pressione ENTER para Reiniciar");
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
    }

    glutSwapBuffers();
}

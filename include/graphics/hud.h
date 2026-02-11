#pragma once

#include <GL/glew.h>

enum WeaponState
{
    W_IDLE,
    W_FIRE_1,
    W_FIRE_2,
    W_RETURN,
    W_PUMP,
    W_RELOAD_1,
    W_RELOAD_2,
    W_RELOAD_3
};

struct HudTextures
{
    GLuint texHudFundo = 0;
    GLuint texGunHUD = 0;

    GLuint texGunDefault = 0;
    GLuint texGunFire1 = 0;
    GLuint texGunFire2 = 0;
    GLuint texGunReload1 = 0;
    GLuint texGunReload2 = 0;

    GLuint texDamage = 0;
    GLuint texHealthOverlay = 0;
};

struct HudState
{
    int playerHealth = 100;
    int currentAmmo = 0;
    int reserveAmmo = 0;

    float damageAlpha = 0.0f;
    float healthAlpha = 0.0f;

    WeaponState weaponState = W_IDLE;
};

void hudRenderAll(
    int screenW,
    int screenH,
    const HudTextures& tex,
    const HudState& state,
    bool showCrosshair,
    bool showWeapon,
    bool showDoomBar);

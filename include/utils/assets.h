#pragma once
#include <GL/glew.h>

struct GameAssets
{
    // texturas
    GLuint texMenuBG = 0;
    GLuint texEndBG = 0;
    GLuint texPorta = 0;
    GLuint texChao1 = 0;
    GLuint texChao2 = 0;
    GLuint texChao3 = 0;
    GLuint texChao4 = 0;
    GLuint texParede1 = 0;
    GLuint texParede2 = 0;
    GLuint texParede3 = 0;
    GLuint texSangue = 0;
    GLuint texLava = 0;
    GLuint texChaoInterno = 0;
    GLuint texParedeInterna = 0;
    GLuint texTeto = 0;
    GLuint texEnemy = 0;
    GLuint texEnemyRage = 0;    // Viu o player
    GLuint texEnemyDamage = 0;  // Leva dano
    GLuint texHealthOverlay = 0; // Tela de cura
    GLuint texHealth = 0;
    GLuint texKey = 0;
    GLuint texGunDefault = 0;
    GLuint texGunFire1 = 0;
    GLuint texGunFire2 = 0;
    GLuint texGunReload1 = 0;
    GLuint texGunReload2 = 0;
    GLuint texDamage = 0;
    GLuint texGunHUD = 0;
    GLuint texHudFundo = 0;
    GLuint texHpHUD = 0;
    GLuint texStaminaHUD = 0;
    GLuint texHandHUD = 0;

    GLuint texEnemies[5]       = {0, 0, 0, 0, 0};
    GLuint texEnemiesRage[5]   = {0, 0, 0, 0, 0};
    GLuint texEnemiesDamage[5] = {0, 0, 0, 0, 0};

    // shaders
    GLuint progSangue = 0;
    GLuint progLava = 0;
    GLuint progExit = 0;

    GLuint texSkydome = 0;
    GLuint texSkydome2 = 0;
    GLuint texSkydome3 = 0;
};

bool loadAssets(GameAssets &a);

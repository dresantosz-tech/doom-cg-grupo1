#include "utils/assets.h"
#include "graphics/texture.h"
#include "graphics/shader.h"
#include <cstdio>

bool loadAssets(GameAssets &a)
{
    a.texMenuBG = carregaTextura("assets/menu_bg.png");
    a.texEndBG = carregaTextura("assets/end_bg.png");
    a.texPorta= carregaTextura("assets/finalDoor.png");
    a.texChao1 = carregaTextura("assets/firstMapGround.png");
    a.texChao2 = carregaTextura("assets/secondMapGround.png");
    a.texChao3 = carregaTextura("assets/lastMapGround.png");
    a.texChao4 = carregaTextura("assets/lastMapGround2.png");
    a.texParede1 = carregaTextura("assets/firstMapWall.png");
    a.texParede2 = carregaTextura("assets/secondMapWall.png");
    a.texParede3 = carregaTextura("assets/lastMapWall.png");
    a.texSangue = carregaTextura("assets/damage.png");
    //a.texLava = carregaTextura("assets/lastMapGround2.png");
    // a.texChaoInterno = carregaTextura("assets/secondMapGround.png");
    // a.texParedeInterna = carregaTextura("assets/firstMapGround.png");
    //a.texTeto1 = carregaTextura("assets/lastMapGround.png");
    a.texTeto2 = carregaTextura("assets/teto.png");
    a.texTeto3 = carregaTextura("assets/teto2.png");

    a.progSangue = criaShader("shaders/blood.vert", "shaders/blood.frag");
    //a.progLava = criaShader("shaders/lava.vert", "shaders/lava.frag");

    // --- INIMIGO 0 ('E') ---
    a.texEnemies[0] = carregaTextura("assets/enemies/enemy.png");
    // a.texEnemiesRage[0] = a.texEnemies[0];
    // a.texEnemiesDamage[0] = a.texEnemies[0];

    // --- INIMIGO 1 ('F') ---
    a.texEnemies[1] = carregaTextura("assets/enemies/enemy2.png");
    // a.texEnemiesRage[1] = a.texEnemies[1];
    // a.texEnemiesDamage[1] = a.texEnemies[1];

    // --- INIMIGO 2 ('G') ---
    a.texEnemies[2] = carregaTextura("assets/enemies/enemy3.png");
    // a.texEnemiesRage[2] = a.texEnemies[2];
    // a.texEnemiesDamage[2] = a.texEnemies[2];

    // --- INIMIGO 3 ('I') ---
    a.texEnemies[3] = carregaTextura("assets/enemies/enemy4.png");
    // a.texEnemiesRage[3] = a.texEnemies[3];
    // a.texEnemiesDamage[3] = a.texEnemies[3];
    // --- INIMIGO 4 ('J') ---
    a.texEnemies[4] = carregaTextura("assets/enemies/enemy5.png");
    // a.texEnemiesRage[4] = a.texEnemies[4];
    // a.texEnemiesDamage[4] = a.texEnemies[4];

    a.texHealthOverlay = carregaTextura("assets/heal.png");
    a.texGunDefault = carregaTextura("assets/hand-POV.png");
    // a.texGunFire1 = a.texGunDefault;
    // a.texGunFire2 = a.texGunDefault;
    // a.texGunReload1 = a.texGunDefault;
    // a.texGunReload2 = a.texGunDefault;
    a.texDamage = carregaTextura("assets/damage.png");

    a.texHealth = carregaTextura("assets/health.png");
    a.texKey = carregaTextura("assets/key.png");

    a.texSkydome = carregaTextura("assets/Va4wUMQ.png");

    a.texGunHUD = a.texGunDefault;
    a.texHudFundo = carregaTextura("assets/hp-HUD.png");
    a.texStaminaHUD = carregaTextura("assets/stamina-HUD.png");

    if (!a.texChao1 || !a.texChao2 || !a.texChao3 || !a.texChao4 || !a.texParede1 || 
        !a.texParede2 || !a.texParede3 || !a.texSangue || !a.texLava || !a.progSangue ||
        !a.progLava || !a.texHealth || !a.texGunDefault || !a.texGunFire1 ||
        !a.texGunFire2 || !a.texSkydome || !a.texGunReload1 || !a.texGunReload2 ||
        !a.texDamage || !a.texKey || !a.texHealthOverlay || !a.texEnemies[0] ||
        !a.texEnemiesRage[0] || !a.texEnemiesDamage[0] || !a.texEnemies[1] ||
        !a.texEnemiesRage[1] || !a.texEnemiesDamage[1] || !a.texEnemies[2] ||
        !a.texEnemiesRage[2] || !a.texEnemiesDamage[2] || !a.texGunHUD || !a.texHudFundo || !a.texMenuBG)
    {
        std::printf("ERRO: falha ao carregar algum asset (textura/shader).\n");
        return false;
    }
    return true;
}

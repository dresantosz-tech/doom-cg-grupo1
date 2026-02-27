#include "graphics/hud.h"
#include "graphics/ui_text.h"
#include <GL/glew.h>
#include <GL/glut.h>
#include <string>
#include <cstdlib>
#include <cmath>

static void begin2D(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
}

static void end2D()
{
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
}

// static void drawCrosshair(int w, int h)
// {
//     glPushAttrib(GL_ENABLE_BIT);
//     glDisable(GL_DEPTH_TEST);
//     glDisable(GL_LIGHTING);
//     glDisable(GL_TEXTURE_2D);

//     begin2D(w, h);

//     glColor3f(0.0f, 1.0f, 0.0f);
//     glLineWidth(2.0f);

//     float cx = w / 2.0f;
//     float cy = h / 2.0f;
//     float size = 10.0f;

//     glBegin(GL_LINES);
//     glVertex2f(cx - size, cy); glVertex2f(cx + size, cy);
//     glVertex2f(cx, cy - size); glVertex2f(cx, cy + size);
//     glEnd();

//     end2D();

//     glPopAttrib();
// }

static void drawDamageOverlay(int w, int h, GLuint texDamage, float alpha)
{
    if (alpha <= 0.0f || texDamage == 0)
        return;

    begin2D(w, h);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, texDamage);
    glColor4f(1, 1, 1, alpha);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex2f(0, 0);
    glTexCoord2f(1, 1); glVertex2f((float)w, 0);
    glTexCoord2f(1, 0); glVertex2f((float)w, (float)h);
    glTexCoord2f(0, 0); glVertex2f(0, (float)h);
    glEnd();

    glDisable(GL_BLEND);

    end2D();
}

static void drawHealthOverlay(int w, int h, GLuint texHealth, float alpha)
{
    if (alpha <= 0.0f || texHealth == 0)
        return;

    begin2D(w, h);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, texHealth);
    glColor4f(1, 1, 1, alpha);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex2f(0, 0);
    glTexCoord2f(1, 1); glVertex2f((float)w, 0);
    glTexCoord2f(1, 0); glVertex2f((float)w, (float)h);
    glTexCoord2f(0, 0); glVertex2f(0, (float)h);
    glEnd();

    glDisable(GL_BLEND);

    end2D();
}

static void drawKeyIndicatorTopRight(int w, int h, GLuint texKey, GLuint texNoKey, bool hasKey)
{
    GLuint tex = hasKey ? texKey : texNoKey;
    if (tex == 0)
        return;

    begin2D(w, h);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, tex);
    glColor4f(1, 1, 1, 1);

    float iconH = h * 0.085f;
    float iconW = iconH;
    float margin = h * 0.025f;
    float x = w - iconW - margin;
    float y = h - iconH - margin;

    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex2f(x, y);
    glTexCoord2f(1, 1); glVertex2f(x + iconW, y);
    glTexCoord2f(1, 0); glVertex2f(x + iconW, y + iconH);
    glTexCoord2f(0, 0); glVertex2f(x, y + iconH);
    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);

    end2D();
}

static void drawKeyPickupPrompt(int w, int h, bool showPrompt)
{
    if (!showPrompt)
        return;

    const char *msg = "Pressione E para pegar a chave";
    const float textScale = 0.22f;
    const float textW = uiStrokeTextWidthScaled(msg, textScale);
    const float panelPadX = 20.0f;
    const float panelPadY = 12.0f;
    const float panelH = 46.0f;
    const float panelW = textW + panelPadX * 2.0f;
    const float panelX = (w - panelW) * 0.5f;
    const float panelY = h * 0.16f;

    begin2D(w, h);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.0f, 0.0f, 0.0f, 0.60f);
    glBegin(GL_QUADS);
    glVertex2f(panelX, panelY);
    glVertex2f(panelX + panelW, panelY);
    glVertex2f(panelX + panelW, panelY + panelH);
    glVertex2f(panelX, panelY + panelH);
    glEnd();

    glColor3f(1.0f, 0.95f, 0.35f);
    uiDrawStrokeText(panelX + panelPadX, panelY + panelPadY, msg, textScale);

    glDisable(GL_BLEND);

    end2D();
}

static void drawCompanionPickupPrompt(int w, int h, bool showPrompt)
{
    if (!showPrompt)
        return;

    const char *msg = "Pressione E para obter companheiro foda";
    const float textScale = 0.20f;
    const float textW = uiStrokeTextWidthScaled(msg, textScale);
    const float panelPadX = 20.0f;
    const float panelPadY = 12.0f;
    const float panelH = 46.0f;
    const float panelW = textW + panelPadX * 2.0f;
    const float panelX = (w - panelW) * 0.5f;
    const float panelY = h * 0.10f;

    begin2D(w, h);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.0f, 0.0f, 0.0f, 0.60f);
    glBegin(GL_QUADS);
    glVertex2f(panelX, panelY);
    glVertex2f(panelX + panelW, panelY);
    glVertex2f(panelX + panelW, panelY + panelH);
    glVertex2f(panelX, panelY + panelH);
    glEnd();

    glColor3f(0.55f, 1.0f, 0.55f);
    uiDrawStrokeText(panelX + panelPadX, panelY + panelPadY, msg, textScale);

    glDisable(GL_BLEND);

    end2D();
}

static void drawDoorPrompt(int w, int h, bool showPrompt, bool canUnlock)
{
    if (!showPrompt)
        return;

    const char *msg = canUnlock
        ? "Pressione E para abrir a porta"
        : "Trancado. Use uma chave para destrancar.";

    const float textScale = 0.20f;
    const float textW = uiStrokeTextWidthScaled(msg, textScale);
    const float panelPadX = 20.0f;
    const float panelPadY = 12.0f;
    const float panelH = 46.0f;
    const float panelW = textW + panelPadX * 2.0f;
    const float panelX = (w - panelW) * 0.5f;
    const float panelY = h * 0.22f;

    begin2D(w, h);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.0f, 0.0f, 0.0f, 0.62f);
    glBegin(GL_QUADS);
    glVertex2f(panelX, panelY);
    glVertex2f(panelX + panelW, panelY);
    glVertex2f(panelX + panelW, panelY + panelH);
    glVertex2f(panelX, panelY + panelH);
    glEnd();

    if (canUnlock)
        glColor3f(0.4f, 1.0f, 0.4f);
    else
        glColor3f(1.0f, 0.72f, 0.30f);
    uiDrawStrokeText(panelX + panelPadX, panelY + panelPadY, msg, textScale);

    glDisable(GL_BLEND);
    end2D();
}

static float computeBobbingOffsetY(const HudState& s);

static float lerpFloat(float a, float b, float t)
{
    return a + (b - a) * t;
}

static void drawWeaponHUD(int w, int h, const HudTextures& tex, const HudState& s)
{
    GLuint currentTex = tex.texGunDefault;
    WeaponState ws = s.weaponState;

    if (ws == WeaponState::W_FIRE_1 || ws == WeaponState::W_RETURN) currentTex = tex.texGunFire1;
    else if (ws == WeaponState::W_FIRE_2) currentTex = tex.texGunFire2;
    else if (ws == WeaponState::W_RELOAD_1 || ws == WeaponState::W_RELOAD_3) currentTex = tex.texGunReload1;
    else if (ws == WeaponState::W_RELOAD_2) currentTex = tex.texGunReload2;

    if (currentTex == 0)
        return;

    begin2D(w, h);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, currentTex);
    glColor4f(1, 1, 1, 1);

    float gunH = h * 0.5f;
    float gunW = gunH;
    float x = (w - gunW) / 2.0f;
    float y = 0.0f;

    if (ws != WeaponState::W_IDLE)
    {
        y -= 20.0f;
        x += (float)(std::rand() % 10 - 5);
    }

    y += computeBobbingOffsetY(s);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex2f(x, y);
    glTexCoord2f(1, 1); glVertex2f(x + gunW, y);
    glTexCoord2f(1, 0); glVertex2f(x + gunW, y + gunH);
    glTexCoord2f(0, 0); glVertex2f(x, y + gunH);
    glEnd();

    glDisable(GL_BLEND);

    end2D();
}

static float computeBobbingOffsetY(const HudState& s)
{
    if (!s.isMoving)
        return 0.0f;

    const float pi = 3.14159265f;
    const float walkFreqHz = 2.2f;
    const float sprintFreqHz = 4.0f;
    const float walkAmp = 8.0f;
    const float sprintAmp = 11.0f;

    const float freq = s.isSprinting ? sprintFreqHz : walkFreqHz;
    const float amp = s.isSprinting ? sprintAmp : walkAmp;
    return std::sin(s.gameTime * freq * 2.0f * pi) * amp;
}

static void drawDoomBar(int w, int h, const HudTextures& tex, const HudState& s)
{
    if (tex.texHudFundo == 0)
        return;

    glPushAttrib(GL_ALL_ATTRIB_BITS);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);

    begin2D(w, h);

    float hBar = h * 0.10f;

    // Fundo (tile)
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex.texHudFundo);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    float repeticaoX = 6.0f;
    float repeticaoY = 1.0f;

    glColor3f(1, 1, 1);
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);                 glVertex2f(0, 0);
    glTexCoord2f(repeticaoX, 0);         glVertex2f((float)w, 0);
    glTexCoord2f(repeticaoX, repeticaoY);glVertex2f((float)w, hBar);
    glTexCoord2f(0, repeticaoY);         glVertex2f(0, hBar);
    glEnd();
    glDisable(GL_TEXTURE_2D);

    // bordas
    glLineWidth(3.0f);
    glColor3f(0.7f, 0.7f, 0.75f);
    glBegin(GL_LINES); glVertex2f(0, hBar); glVertex2f((float)w, hBar); glEnd();

    glColor3f(0.2f, 0.2f, 0.25f);
    glBegin(GL_LINES); glVertex2f(w / 2.0f, 0); glVertex2f(w / 2.0f, hBar); glEnd();

    // texto
    float scaleLbl = 0.0018f * hBar;
    //float scaleNum = 0.0035f * hBar;

    float colLbl[3] = {0.3f, 0.2f, 0.8f};
    //float colNum[3] = {0.8f, 0.0f, 0.0f};

    // HEALTH label
    float xTextHealth = w * 0.08f;
    float yLblHealth = hBar * 0.35f;
    glColor3fv(colLbl);
    uiDrawStrokeText(xTextHealth, yLblHealth, "HEALTH", scaleLbl);

    // barra vida
    float barH = hBar * 0.5f;
    float barY = (hBar - barH) / 2.0f;
    float barX = xTextHealth + (w * 0.08f);
    float barMaxW = (w * 0.45f) - barX;

    if (tex.texHealthHudIcon != 0)
    {
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor3f(1, 1, 1);

        float iconH = barH;
        float iconW = iconH;
        float iconX = barX - iconW - 8.0f;
        float iconY = barY;

        glBindTexture(GL_TEXTURE_2D, tex.texHealthHudIcon);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 1); glVertex2f(iconX, iconY);
        glTexCoord2f(1, 1); glVertex2f(iconX + iconW, iconY);
        glTexCoord2f(1, 0); glVertex2f(iconX + iconW, iconY + iconH);
        glTexCoord2f(0, 0); glVertex2f(iconX, iconY + iconH);
        glEnd();

        glDisable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);
    }

    glColor4f(0, 0, 0, 1);
    glBegin(GL_QUADS);
    glVertex2f(barX, barY); glVertex2f(barX + barMaxW, barY);
    glVertex2f(barX + barMaxW, barY + barH); glVertex2f(barX, barY + barH);
    glEnd();

    float pct = (float)s.playerHealth / 100.0f;
    if (pct < 0) pct = 0;
    if (pct > 1) pct = 1;

    if (pct > 0.6f) glColor3f(0.0f, 0.8f, 0.0f);
    else if (pct > 0.3f) glColor3f(1.0f, 0.8f, 0.0f);
    else glColor3f(0.8f, 0.0f, 0.0f);

    glBegin(GL_QUADS);
    glVertex2f(barX, barY);
    glVertex2f(barX + (barMaxW * pct), barY);
    glVertex2f(barX + (barMaxW * pct), barY + barH);
    glVertex2f(barX, barY + barH);
    glEnd();

    // STAMINA label
    float xTextStamina = w * 0.58f;
    float yLblStamina = hBar * 0.35f;
    glColor3fv(colLbl);
    uiDrawStrokeText(xTextStamina, yLblStamina, "STAMINA", scaleLbl);

    // barra stamina
    float stBarX = xTextStamina + (w * 0.10f);
    float stBarMaxW = (w * 0.95f) - stBarX;

    if (tex.texStaminaHudIcon != 0)
    {
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glColor3f(1, 1, 1);

        float iconH = barH;
        float iconW = iconH;
        float iconX = stBarX - iconW - 8.0f;
        float iconY = barY;

        glBindTexture(GL_TEXTURE_2D, tex.texStaminaHudIcon);
        glBegin(GL_QUADS);
        glTexCoord2f(0, 1); glVertex2f(iconX, iconY);
        glTexCoord2f(1, 1); glVertex2f(iconX + iconW, iconY);
        glTexCoord2f(1, 0); glVertex2f(iconX + iconW, iconY + iconH);
        glTexCoord2f(0, 0); glVertex2f(iconX, iconY + iconH);
        glEnd();

        glDisable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);
    }

    glColor4f(0, 0, 0, 1);
    glBegin(GL_QUADS);
    glVertex2f(stBarX, barY); glVertex2f(stBarX + stBarMaxW, barY);
    glVertex2f(stBarX + stBarMaxW, barY + barH); glVertex2f(stBarX, barY + barH);
    glEnd();

    float staminaTarget = (float)s.playerStamina / 100.0f;
    if (staminaTarget < 0) staminaTarget = 0;
    if (staminaTarget > 1) staminaTarget = 1;

    // Suaviza visualmente a barra para evitar "saltos" quando o valor inteiro muda.
    static float staminaSmooth = 1.0f;
    staminaSmooth = lerpFloat(staminaSmooth, staminaTarget, 0.22f);
    float staminaPct = staminaSmooth;

    // Cor dinamica: vermelho (0%) -> amarelo (50%) -> azul/ciano (100%).
    float rCol, gCol, bCol;
    if (staminaPct < 0.5f)
    {
        float t = staminaPct / 0.5f;
        rCol = lerpFloat(0.90f, 1.00f, t);
        gCol = lerpFloat(0.20f, 0.80f, t);
        bCol = lerpFloat(0.10f, 0.00f, t);
    }
    else
    {
        float t = (staminaPct - 0.5f) / 0.5f;
        rCol = lerpFloat(1.00f, 0.00f, t);
        gCol = lerpFloat(0.80f, 0.70f, t);
        bCol = lerpFloat(0.00f, 1.00f, t);
    }

    // Pulso quando stamina baixa.
    if (staminaPct <= 0.30f)
    {
        float pulse = 0.75f + 0.25f * (0.5f + 0.5f * std::sin(s.gameTime * 9.0f));
        rCol *= pulse;
        gCol *= pulse;
        bCol *= pulse;
    }

    glColor3f(rCol, gCol, bCol);

    glBegin(GL_QUADS);
    glVertex2f(stBarX, barY);
    glVertex2f(stBarX + (stBarMaxW * staminaPct), barY);
    glVertex2f(stBarX + (stBarMaxW * staminaPct), barY + barH);
    glVertex2f(stBarX, barY + barH);
    glEnd();

    // arma ícone
    // if (tex.texGunHUD != 0)
    // {
    //     glEnable(GL_TEXTURE_2D);
    //     glEnable(GL_BLEND);
    //     glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //     glColor3f(1, 1, 1);

    //     float iconSize = hBar * 1.5f;
    //     float iconY = (hBar - iconSize) / 2.0f + (hBar * 0.1f);

    //     glBindTexture(GL_TEXTURE_2D, tex.texGunHUD);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //     float weaponWidth = iconSize * 2.2f;
    //     float xIconGun = (w * 0.75f) - (weaponWidth / 2.0f);

    //     glBegin(GL_QUADS);
    //     glTexCoord2f(0, 1); glVertex2f(xIconGun, iconY);
    //     glTexCoord2f(1, 1); glVertex2f(xIconGun + weaponWidth, iconY);
    //     glTexCoord2f(1, 0); glVertex2f(xIconGun + weaponWidth, iconY + iconSize);
    //     glTexCoord2f(0, 0); glVertex2f(xIconGun, iconY + iconSize);
    //     glEnd();

    //     glDisable(GL_BLEND);
    //     glDisable(GL_TEXTURE_2D);

    //     // AMMO número + label
    //     float xAmmoBlock = xIconGun + weaponWidth + 10.0f;
    //     float yNum = hBar * 0.50f;
    //     float xNum = xAmmoBlock + 5.0f;

    //     glColor3fv(colNum);
    //     glPushMatrix();
    //     glTranslatef(xNum, yNum, 0);
    //     glScalef(scaleNum, scaleNum, 1);
    //     {
    //         std::string sAmmo = std::to_string(s.currentAmmo);
    //         for (char c : sAmmo) glutStrokeCharacter(GLUT_STROKE_MONO_ROMAN, c);
    //     }
    //     glPopMatrix();

    //     glColor3fv(colLbl);
    //     uiDrawStrokeText(xAmmoBlock, hBar * 0.20f, "AMMO", scaleLbl);
    // }

    end2D();
    glPopAttrib();
}

void hudRenderAll(
    int screenW,
    int screenH,
    const HudTextures& tex,
    const HudState& state,
    bool showCrosshair,
    bool showWeapon,
    bool showDoomBar)
{
    // Ordem: arma -> barra -> mira -> overlays
    if (showWeapon)  drawWeaponHUD(screenW, screenH, tex, state);
    if (showDoomBar) drawDoomBar(screenW, screenH, tex, state);
    drawKeyIndicatorTopRight(screenW, screenH, tex.texKeyIcon, tex.texNoKeyIcon, state.hasKey);
    drawKeyPickupPrompt(screenW, screenH, state.showKeyPickupPrompt);
    drawCompanionPickupPrompt(screenW, screenH, state.showCompanionPickupPrompt);
    drawDoorPrompt(screenW, screenH, state.showDoorPrompt, state.canUnlockDoor);

    (void)showCrosshair;

    drawDamageOverlay(screenW, screenH, tex.texDamage, state.damageAlpha);
    drawHealthOverlay(screenW, screenH, tex.texHealthOverlay, state.healthAlpha);
}

#pragma once

#include <GL/glut.h>

#include <vector>
#include <cstdlib>

// Tipo correto usado pelo menu
struct FireParticle {
    float x, y;
    float velY;
    float life;
    float size;
    float r, g, b;
};

struct FireSystem {
    std::vector<FireParticle> particles;
};

// ========= helpers de texto (precisam ser inline se ficarem no header) =========
inline void uiDrawStrokeText(float x, float y, const char* text, float scale)
{
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(scale, scale, 1.0f);
    glLineWidth(2.0f);

    for (const char* c = text; *c; ++c)
        glutStrokeCharacter(GLUT_STROKE_ROMAN, *c);

    glPopMatrix();
}

inline float uiStrokeTextWidth(const char* text)
{
    float w = 0.0f;
    for (const char* c = text; *c; ++c)
        w += glutStrokeWidth(GLUT_STROKE_ROMAN, *c);
    return w;
}

inline float uiStrokeTextWidthScaled(const char* text, float scale)
{
    return uiStrokeTextWidth(text) * scale;
}

// atualiza (spawna + move + remove)
void fireUpdate(FireSystem& fs, int screenW, int screenH);

// desenha (assume que o caller já está em 2D)
void fireRender(const FireSystem& fs);

inline void fireDraw(const FireSystem& fs)
{
    glDisable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // aditivo

    glBegin(GL_QUADS);
    for (const auto& f : fs.particles) {
        glColor4f(f.r, f.g, f.b, f.life * 0.6f);
        float t = f.size;
        glVertex2f(f.x - t, f.y - t);
        glVertex2f(f.x + t, f.y - t);
        glVertex2f(f.x + t, f.y + t);
        glVertex2f(f.x - t, f.y + t);
    }
    glEnd();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

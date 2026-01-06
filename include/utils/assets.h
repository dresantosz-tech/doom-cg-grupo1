#pragma once
#include <GL/glew.h>

struct GameAssets
{
    // texturas
    GLuint texChao = 0;
    GLuint texParede = 0;
    GLuint texSangue = 0;
    GLuint texLava = 0;
    GLuint texChaoInterno = 0;
    GLuint texParedeInterna = 0;
    GLuint texTeto = 0;

    // skyboxes
    GLuint texSkyRight = 0;
    GLuint texSkyLeft = 0;
    GLuint texSkyTop = 0;
    GLuint texSkyBottom = 0;
    GLuint texSkyFront = 0;
    GLuint texSkyBack = 0;

    // shaders
    GLuint progSangue = 0;
    GLuint progLava = 0;
};

bool loadAssets(GameAssets &a);

#include <GL/glew.h>
#include <GL/glut.h>
#include <cstdlib>

#include "input/input.h"
#include "input/keystate.h"
#include "core/window.h"
#include "graphics/menu.h"
#include "core/game.h"
#include "core/entities.h"

void keyboard(unsigned char key, int, int)
{
    const GameState state = gameGetState();

    if (key == 27)
        std::exit(0);

    if (state == GameState::MENU_INICIAL)
    {
        if (key == 13 && !menuMeltIsActive())
            menuMeltRequestStart();
        return;
    }

    if (state == GameState::GAME_OVER)
    {
        if (key == 13)
        {
            gameReset();
            gameSetState(GameState::JOGANDO);
        }
        return;
    }

    if (state == GameState::VITORIA)
    {
        if (key == 13)
        {
            gameReset();
            gameSetState(GameState::JOGANDO);
        }
        return;
    }

    if (state == GameState::PAUSADO)
    {
        if (key == 'p' || key == 'P')
            gameSetState(GameState::JOGANDO);
        return;
    }

    if (state == GameState::JOGANDO)
    {
        if (key == 'p' || key == 'P')
        {
            gameSetState(GameState::PAUSADO);
            keyW = keyA = keyS = keyD = false;
            keyShift = false;
            return;
        }

        switch (key)
        {
        case 'w':
        case 'W':
            keyW = true;
            break;
        case 's':
        case 'S':
            keyS = true;
            break;
        case 'a':
        case 'A':
            keyA = true;
            break;
        case 'd':
        case 'D':
            keyD = true;
            break;
        case 'e':
        case 'E':
            playerTryInteract();
            break;
        }
    }
}

void keyboardUp(unsigned char key, int, int)
{
    switch (key)
    {
    case 'w':
    case 'W':
        keyW = false;
        break;
    case 's':
    case 'S':
        keyS = false;
        break;
    case 'a':
    case 'A':
        keyA = false;
        break;
    case 'd':
    case 'D':
        keyD = false;
        break;
    }

    if ((key == 13 || key == '\r') && (glutGetModifiers() & GLUT_ACTIVE_ALT))
        altFullScreen();
}

void specialDown(int key, int, int)
{
#if defined(GLUT_KEY_SHIFT_L) && defined(GLUT_KEY_SHIFT_R)
    if (key == GLUT_KEY_SHIFT_L || key == GLUT_KEY_SHIFT_R)
        keyShift = true;
#else
    (void)key;
#endif
}

void specialUp(int key, int, int)
{
#if defined(GLUT_KEY_SHIFT_L) && defined(GLUT_KEY_SHIFT_R)
    if (key == GLUT_KEY_SHIFT_L || key == GLUT_KEY_SHIFT_R)
        keyShift = false;
#else
    (void)key;
#endif
}

void mouseClick(int button, int state, int x, int y)
{
    (void)x;
    (void)y;
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {}
}

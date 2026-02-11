#pragma once

#include <string>
#include "graphics/fire_particles.h"

void menuRender(int screenW, int screenH, float tempo, FireSystem& fire,
                const std::string& title, const std::string& subTitle);


void pauseMenuRender(int screenW, int screenH, float tempo);

#ifndef RENDER_H
#define RENDER_H

#include "game_globals.h"

void renderText(const std::string& text, int x, int y, SDL_Color color);
void renderMenu();
void renderInfoScreen();
void renderCards();
void renderPauseScreen();
void renderGameOverScreen();
void renderTimeBar();

#endif // RENDER_H


#ifndef INPUT_H
#define INPUT_H
#include "game_globals.h"
#include "game_types.h"

void handleMenuClick(int mouseX, int mouseY);
void handleCardClick(int mouseX, int mouseY);
void handlePauseClick(int mouseX, int mouseY);
bool isMouseOverButton(int mouseX, int mouseY, const Button& button);

#endif // INPUT_H


#ifndef INPUT_H
#define INPUT_H
#include "global.h"
#include "types.h"

void handleMenuClick(int mouseX, int mouseY);
void handleCardClick(int mouseX, int mouseY);
void handlePauseClick(int mouseX, int mouseY);
bool isMouseOverButton(int mouseX, int mouseY, const Button& button);

#endif // INPUT_H


#ifndef TYPES_H
#define TYPES_H

#include "global.h"
#include <SDL.h>

enum class GameState { MENU, PLAYING, PAUSED, GAME_OVER, INFO };

struct Card {
    SDL_Rect rect;
    int value;
    bool isFaceUp = false;
    bool isMatched = false;
    SDL_Texture* frontTexture = nullptr;
    SDL_Texture* backTexture = nullptr;
};

struct Button {
    SDL_Rect rect;
    Button(int x, int y, int w, int h) : rect{x, y, w, h} {}
};

#endif // TYPES_H


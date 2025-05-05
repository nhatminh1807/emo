#ifndef GAME_TYPES_H
#define GAME_TYPES_H

#include "game_globals.h"
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

#endif // GAME_TYPES_H


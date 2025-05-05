#ifndef ASSET_H
#define ASSET_H

#include "game_globals.h"

bool initSDL();
bool loadMedia();
SDL_Texture* loadTexture(const string& path);
void close();

#endif // ASSET_H


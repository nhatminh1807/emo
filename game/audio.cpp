#include "audio.h"
#include "global.h"

void playSound(Mix_Chunk* sound) {
    if (isSoundOn && sound) {
        if (Mix_Playing(-1) < 2) {
            Mix_PlayChannel(-1, sound, 0);
        }
    }
}

void playClickSound() {
    if (isSoundOn && clickSound) {
        Mix_PlayChannel(-1, clickSound, 0);
    }
}


#ifndef GLOBAL_H
#define GLOBAL_H

#include "types.h"
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream>
#include <cstdlib>

using namespace std;

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 600;
const int CARD_WIDTH = 80;
const int CARD_HEIGHT = 80;
const int MAX_LEVEL = 3;
const Uint32 FLIP_DELAY_MS = 1000;
const Uint32 LAST_PAIR_DELAY_MS = 1000;

extern GameState currentState;

extern vector<Card> cards;
extern vector<Card*> flippedCards;
extern vector<SDL_Texture*> emojiTextures;

extern SDL_Texture* backTexture;
extern SDL_Texture* matchedTexture;
extern SDL_Texture* backgroundTexture;
extern SDL_Texture* menuTexture;
extern SDL_Texture* infoTexture;
extern SDL_Texture* soundOffTexture;
extern SDL_Texture* pauseWindowTexture;
extern SDL_Texture* gameOverWindowTexture;

extern Button startButton;
extern Button infoButton;
extern Button soundButton;
extern Button exitButton;
extern Button pauseButton;
extern Button continueButton;
extern Button homeButton;
extern Button replayButton;

extern Mix_Chunk* flipSound;
extern Mix_Chunk* matchedSound;
extern Mix_Chunk* wrongSound;
extern Mix_Music* bgMusic;
extern Mix_Chunk* clickSound;
extern Mix_Chunk* loseSound;
extern Mix_Chunk* x2Sound;

extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern TTF_Font* font;

extern bool isSoundOn;
extern int level;
extern int pairsFound;
extern int totalPairs;
extern int score;
extern int currentLevelScore;
extern int previousScore;
extern int highestScore;
extern Uint32 flipTime;
extern bool isWaiting;
extern bool isLastPairMatched;
extern Uint32 lastPairTime;
extern Uint32 levelStartTime;
extern Uint32 pauseStartTime;
extern vector<int> levelTimes;
extern Uint32 remainingTime;
extern bool isTimeUp;
extern int BOMB_CARD_VALUE;
extern int X2_CARD_VALUE;
extern int normalPairs;

#endif // GLOBAL_H


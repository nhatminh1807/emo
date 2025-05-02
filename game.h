#ifndef GAME_H
#define GAME_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream>

using namespace std;

// Constants
const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 600;
const int CARD_WIDTH = 80;
const int CARD_HEIGHT = 80;
const int MAX_LEVEL = 2;
const Uint32 FLIP_DELAY_MS = 1000;
const Uint32 LAST_PAIR_DELAY_MS = 1000;

// Game state
enum class GameState { MENU, PLAYING, PAUSED, GAME_OVER, INFO };
extern GameState currentState;

// Game structs
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
    explicit Button(int x, int y, int w, int h) : rect{x, y, w, h} {}
};

// Game variables
extern vector<Card> cards;
extern vector<SDL_Texture*> emojiTextures;
extern SDL_Texture* backTexture;
extern SDL_Texture* backgroundTexture;
extern SDL_Texture* menuTexture;
extern SDL_Texture* infoTexture;
extern SDL_Texture* soundOffTexture;
extern SDL_Texture* pauseWindowTexture;
extern SDL_Texture* gameOverWindowTexture;
extern SDL_Texture* matchedTexture;

extern Mix_Chunk* flipSound;
extern Mix_Chunk* matchedSound;
extern Mix_Chunk* wrongSound;
extern Mix_Music* bgMusic;
extern Mix_Chunk* clickSound;

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

extern vector<Card*> flippedCards;

extern Uint32 levelStartTime;
extern Uint32 pauseStartTime;
extern vector<int> levelTimes;
extern Uint32 remainingTime;
extern bool isTimeUp;

// UI Elements
extern Button startButton;
extern Button infoButton;
extern Button soundButton;
extern Button pauseButton;
extern Button continueButton;
extern Button homeButton;
extern Button replayButton;

// SDL Resources
extern SDL_Window* window;
extern SDL_Renderer* renderer;
extern TTF_Font* font;

// Function declarations
bool init();
bool loadMedia();
int readHighestScore();
void writeHighestScore(int score);
SDL_Texture* loadTexture(const string& path);
void renderText(const string& text, int x, int y, SDL_Color color);
bool isMouseOverButton(int mouseX, int mouseY, const Button& button);
void renderMenu();
void renderInfoScreen();
void initLevel();
void renderCards();
void renderPauseScreen();
void renderGameOverScreen();
void renderTimeBar();
void handleMenuClick(int mouseX, int mouseY);
void handleCardClick(int mouseX, int mouseY);
void handlePauseClick(int mouseX, int mouseY);
void close();
void playSound(Mix_Chunk* sound);
void playClickSound();
void checkAndUpdateHighScore();

#endif // GAME_H

#include "game_globals.h"

GameState currentState = GameState::MENU;

vector<Card> cards;
vector<Card*> flippedCards;
vector<SDL_Texture*> emojiTextures;

SDL_Texture* backTexture = nullptr;
SDL_Texture* matchedTexture = nullptr;
SDL_Texture* backgroundTexture = nullptr;
SDL_Texture* menuTexture = nullptr;
SDL_Texture* infoTexture = nullptr;
SDL_Texture* soundOffTexture = nullptr;
SDL_Texture* pauseWindowTexture = nullptr;
SDL_Texture* gameOverWindowTexture = nullptr;

Mix_Chunk* flipSound = nullptr;
Mix_Chunk* matchedSound = nullptr;
Mix_Chunk* wrongSound = nullptr;
Mix_Music* bgMusic = nullptr;
Mix_Chunk* clickSound = nullptr;
Mix_Chunk* loseSound = nullptr;
Mix_Chunk* x2Sound = nullptr;

bool isSoundOn = true;
int level = 1;
int pairsFound = 0;
int totalPairs = 0;
int score = 0;
int currentLevelScore = 0;
int previousScore = 0;
int highestScore = 0;
Uint32 flipTime = 0;
bool isWaiting = false;
bool isLastPairMatched = false;
Uint32 lastPairTime = 0;
Uint32 levelStartTime = 0;
Uint32 pauseStartTime = 0;
vector<int> levelTimes = {10, 60, 90};
Uint32 remainingTime = 0;
bool isTimeUp = false;
int BOMB_CARD_VALUE = 2;
int X2_CARD_VALUE = 3;
int normalPairs = 0;

Button startButton(410, 480, 130, 70);
Button infoButton(815, 550, 85, 55);
Button soundButton(2, 543, 89, 58);
Button exitButton(815, 5, 85, 55);
Button pauseButton(2, 5, 89, 58);
Button continueButton(0, 0, 200, 80);
Button homeButton(0, 0, 80, 80);
Button replayButton(0, 0, 80, 80);

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
TTF_Font* font = nullptr;


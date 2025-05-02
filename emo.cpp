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

const int SCREEN_WIDTH = 900;
const int SCREEN_HEIGHT = 600;
const int CARD_WIDTH = 80;
const int CARD_HEIGHT = 80;
const int MAX_LEVEL = 2;
const Uint32 FLIP_DELAY_MS = 1000;
const Uint32 LAST_PAIR_DELAY_MS = 1000;

// Game state
enum class GameState { MENU, PLAYING, PAUSED, GAME_OVER, INFO };
GameState currentState = GameState::MENU;

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
vector<Card> cards;
vector<SDL_Texture*> emojiTextures;
SDL_Texture* backTexture = nullptr;
SDL_Texture* backgroundTexture = nullptr;
SDL_Texture* menuTexture = nullptr;
SDL_Texture* infoTexture = nullptr;
SDL_Texture* soundOffTexture = nullptr;
SDL_Texture* pauseWindowTexture = nullptr;
SDL_Texture* gameOverWindowTexture = nullptr;
SDL_Texture* matchedTexture = nullptr;

Mix_Chunk* flipSound = nullptr;
Mix_Chunk* matchedSound = nullptr;
Mix_Chunk* wrongSound = nullptr;
Mix_Music* bgMusic = nullptr;

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

vector<Card*> flippedCards;

Uint32 levelStartTime;
Uint32 pauseStartTime;
vector<int> levelTimes = {20, 75, 120};
Uint32 remainingTime;
bool isTimeUp = false;

// UI Elements
Button startButton(410, 480, 130, 70);
Button infoButton(815, 550, 85, 55);
Button soundButton(2, 543, 89, 58);
Button pauseButton(2, 5, 89, 58);
Button continueButton(0, 0, 200, 80);
Button homeButton(0, 0, 80, 80);
Button replayButton(0, 0, 80, 80);

// SDL Resources
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
TTF_Font* font = nullptr;

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

bool isMouseOverButton(int mouseX, int mouseY, const Button& button) {
    return (mouseX >= button.rect.x) &&
           (mouseX <= button.rect.x + button.rect.w) &&
           (mouseY >= button.rect.y) &&
           (mouseY <= button.rect.y + button.rect.h);
}

int readHighestScore() {
    ifstream file("highscore.txt");
    int score = 0;
    if (file.is_open()) {
        file >> score;
        file.close();
    }
    return score;
}

void writeHighestScore(int score) {
    ofstream file("highscore.txt");
    if (file.is_open()) {
        file << score;
        file.close();
    }
}

void checkAndUpdateHighScore() {
    if (score > highestScore) {
        highestScore = score;
        writeHighestScore(highestScore);
    }
}

// Main initialization
bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "SDL initialization failed: " << SDL_GetError() << endl;
        return false;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

    window = SDL_CreateWindow("Memory Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                             SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return false;

    if (TTF_Init() == -1 || !(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        cerr << "TTF or IMG initialization failed" << endl;
        return false;
    }

    font = TTF_OpenFont("fredoka.ttf", 36);
    if (!font) return false;

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        cerr << "MIXER initialization failed: " << Mix_GetError() << endl;
        return false;
    }

    return true;
}

bool loadMedia() {
    const vector<string> emojiFiles = {
        "emojis/smile.png", "emojis/heart.png", "emojis/star.png", "emojis/cat.png",
        "emojis/dog.png", "emojis/sun.png", "emojis/moon.png", "emojis/tree.png"
    };

    for (const auto& file : emojiFiles) {
        SDL_Texture* texture = loadTexture(file);
        if (!texture) return false;
        emojiTextures.push_back(texture);
    }

    backTexture = loadTexture("emojis/back.png");
    matchedTexture = loadTexture("emojis/matched.png");
    backgroundTexture = loadTexture("background.png");
    menuTexture = loadTexture("buttons/main.png");
    infoTexture = loadTexture("buttons/infor.png");
    soundOffTexture = loadTexture("buttons/muted.png");
    pauseWindowTexture = loadTexture("buttons/window.png");
    gameOverWindowTexture = loadTexture("buttons/window2.png");

    if (!(backTexture && matchedTexture && backgroundTexture && menuTexture &&
            infoTexture && soundOffTexture && pauseWindowTexture && gameOverWindowTexture)) {
        return false;
    }

    flipSound = Mix_LoadWAV("sounds/flip.wav");
    matchedSound = Mix_LoadWAV("sounds/matched.wav");
    wrongSound = Mix_LoadWAV("sounds/wrong.wav");
    bgMusic = Mix_LoadMUS("sounds/bgsound.wav");

    if (!flipSound || !matchedSound || !wrongSound || !bgMusic) {
        cerr << "Failed to load sound files! Error: " << Mix_GetError() << endl;
        return false;
    }

    if (isSoundOn) {
        Mix_PlayMusic(bgMusic, -1);
        Mix_VolumeMusic(5);
    }

    return true;
}

void playSound(Mix_Chunk* sound) {
    if (isSoundOn && sound) {
        if (Mix_Playing(-1) < 2) {
            Mix_PlayChannel(-1, sound, 0);
        }
    }
}

SDL_Texture* loadTexture(const string& path) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) return nullptr;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

void renderText(const string& text, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture) {
        SDL_Rect rect = {x, y, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
        SDL_DestroyTexture(texture);
    }
    SDL_FreeSurface(surface);
}

void renderMenu() {
    SDL_RenderCopy(renderer, menuTexture, nullptr, nullptr);
    if (!isSoundOn) {
        SDL_RenderCopy(renderer, soundOffTexture, nullptr, &soundButton.rect);
    }
}

void renderInfoScreen() {
    SDL_RenderCopy(renderer, menuTexture, nullptr, nullptr);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
    SDL_RenderFillRect(renderer, nullptr);

    int infoWidth, infoHeight;
    SDL_QueryTexture(infoTexture, nullptr, nullptr, &infoWidth, &infoHeight);
    SDL_Rect infoRect = {
        (SCREEN_WIDTH - infoWidth) / 2,
        (SCREEN_HEIGHT - infoHeight) / 2,
        infoWidth,
        infoHeight
    };
    SDL_RenderCopy(renderer, infoTexture, nullptr, &infoRect);
}

void renderTimeBar() {
    if (currentState == GameState::PAUSED) {
        remainingTime = levelTimes[level-1] - (pauseStartTime - levelStartTime)/1000;
    } else {
        remainingTime = levelTimes[level-1] - (SDL_GetTicks() - levelStartTime)/1000;
    }

    if (remainingTime <= 0) {
        remainingTime = 0;
        isTimeUp = true;
        currentState = GameState::GAME_OVER;
    }

    const int TIME_BAR_WIDTH = 470;
    const int TIME_BAR_HEIGHT = 30;
    const int TIME_BAR_X = (SCREEN_WIDTH - TIME_BAR_WIDTH) / 2;
    const int TIME_BAR_Y = 24;

    SDL_Rect timeBarBack = {TIME_BAR_X, TIME_BAR_Y, TIME_BAR_WIDTH, TIME_BAR_HEIGHT};
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderFillRect(renderer, &timeBarBack);

    float timeRatio = (float)remainingTime / levelTimes[level-1];
    int currentTimeWidth = TIME_BAR_WIDTH * timeRatio;

    SDL_Rect timeBar = {TIME_BAR_X, TIME_BAR_Y, currentTimeWidth, TIME_BAR_HEIGHT};

    if (timeRatio > 0.6) {
        SDL_SetRenderDrawColor(renderer, 76, 187, 23, 255);
    } else if (timeRatio > 0.3) {
        SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    }

    SDL_RenderFillRect(renderer, &timeBar);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &timeBarBack);
}

void initLevel() {
    cards.clear();
    flippedCards.clear();
    pairsFound = 0;
    isWaiting = false;
    flipTime = 0;
    currentLevelScore = 0;
    previousScore = score;
    isTimeUp = false;
    isLastPairMatched = false;
    levelStartTime = SDL_GetTicks();

    int gridSize = level * 2;
    totalPairs = (gridSize * gridSize) / 2;

    vector<int> values;
    for (int i = 0; i < totalPairs; ++i) {
        values.push_back(i % emojiTextures.size());
        values.push_back(i % emojiTextures.size());
    }

    shuffle(values.begin(), values.end(), mt19937{random_device{}()});

    constexpr int cardSpacing = 10;
    int totalWidth = gridSize * CARD_WIDTH + (gridSize - 1) * cardSpacing;
    int totalHeight = gridSize * CARD_HEIGHT + (gridSize - 1) * cardSpacing;
    int startX = (SCREEN_WIDTH - totalWidth) / 2;
    int startY = (SCREEN_HEIGHT - totalHeight) / 2;

    for (int row = 0; row < gridSize; ++row) {
        for (int col = 0; col < gridSize; ++col) {
            int index = row * gridSize + col;
            if (index < values.size()) {
                Card card{
                    {startX + col * (CARD_WIDTH + cardSpacing),
                     startY + row * (CARD_HEIGHT + cardSpacing),
                     CARD_WIDTH, CARD_HEIGHT},
                    values[index],
                    false,
                    false,
                    emojiTextures[values[index]],
                    backTexture
                };
                cards.push_back(card);
            }
        }
    }
}

void renderCards() {
    for (const auto& card : cards) {
        if (card.isMatched) {
            SDL_RenderCopy(renderer, matchedTexture, nullptr, &card.rect);
        } else {
            SDL_RenderCopy(renderer, card.isFaceUp ? card.frontTexture : card.backTexture,
                         nullptr, &card.rect);
        }
    }
}

void renderPauseScreen() {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
    SDL_RenderFillRect(renderer, nullptr);

    int windowWidth, windowHeight;
    SDL_QueryTexture(pauseWindowTexture, nullptr, nullptr, &windowWidth, &windowHeight);
    SDL_Rect windowRect = {
        (SCREEN_WIDTH - windowWidth) / 2,
        (SCREEN_HEIGHT - windowHeight) / 2,
        windowWidth,
        windowHeight
    };
    SDL_RenderCopy(renderer, pauseWindowTexture, nullptr, &windowRect);

    homeButton.rect = {windowRect.x + 70, windowRect.y + windowRect.h / 2 + 5, 80, 80};
    continueButton.rect = {windowRect.x + windowRect.w / 2 - 100, windowRect.y + windowRect.h / 2 - 5, 200, 80};
    replayButton.rect = {windowRect.x + windowRect.w - 150, windowRect.y + windowRect.h / 2 + 5, 80, 80};
}

void renderGameOverScreen() {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
    SDL_RenderFillRect(renderer, nullptr);

    int windowWidth, windowHeight;
    SDL_QueryTexture(gameOverWindowTexture, nullptr, nullptr, &windowWidth, &windowHeight);
    SDL_Rect windowRect = {
        (SCREEN_WIDTH - windowWidth) / 2,
        (SCREEN_HEIGHT - windowHeight) / 2,
        windowWidth,
        windowHeight
    };
    SDL_RenderCopy(renderer, gameOverWindowTexture, nullptr, &windowRect);

    homeButton.rect = {windowRect.x + 150, windowRect.y + windowRect.h - 180, 80, 80};
    replayButton.rect = {windowRect.x + windowRect.w - 230, windowRect.y + windowRect.h - 180, 80, 80};

    SDL_Color brown = {39, 32, 22, 255};
    SDL_Color yellow = {219, 179, 73, 255};
    renderText("Final Score: " + to_string(score),
              windowRect.x + (windowWidth - 200) / 2 - 30,
              windowRect.y + windowRect.h / 2 - 70,
              brown);
    renderText("HIGHEST SCORE: " + to_string(highestScore),
              windowRect.x + (windowWidth - 200) / 2 - 50,
              windowRect.y + windowRect.h / 2 - 170,
              yellow);
}

void handleMenuClick(int mouseX, int mouseY) {
    if (isMouseOverButton(mouseX, mouseY, startButton)) {
        currentState = GameState::PLAYING;
        initLevel();
    } else if (isMouseOverButton(mouseX, mouseY, infoButton)) {
        currentState = GameState::INFO;
    } else if (isMouseOverButton(mouseX, mouseY, soundButton)) {
        isSoundOn = !isSoundOn;
        if (isSoundOn) {
            Mix_ResumeMusic();
        } else {
            Mix_PauseMusic();
        }
    }
}

void handleCardClick(int mouseX, int mouseY) {
    if (isWaiting || currentState != GameState::PLAYING || isTimeUp || isLastPairMatched) return;

    for (auto& card : cards) {
        SDL_Point mousePos = {mouseX, mouseY};
        if (!card.isFaceUp && !card.isMatched &&
            SDL_PointInRect(&mousePos, &card.rect)) {

            card.isFaceUp = true;
            flippedCards.push_back(&card);
            playSound(flipSound);

            if (flippedCards.size() == 2) {
                if (flippedCards[0]->value == flippedCards[1]->value) {
                    playSound(matchedSound);
                    flippedCards[0]->isMatched = true;
                    flippedCards[1]->isMatched = true;
                    pairsFound++;
                    currentLevelScore += 10 * level;
                    score += 10 * level;
                    flippedCards.clear();

                    if (pairsFound == totalPairs) {
                        isLastPairMatched = true;
                        lastPairTime = SDL_GetTicks();
                    }
                } else {
                    playSound(wrongSound);
                    currentLevelScore = max(0, currentLevelScore - 1);
                    score = max(0, score - 1);
                    isWaiting = true;
                    flipTime = SDL_GetTicks();
                }
            }
            break;
        }
    }
}

void handlePauseClick(int mouseX, int mouseY) {
    if (isMouseOverButton(mouseX, mouseY, continueButton)) {
        currentState = GameState::PLAYING;
        levelStartTime += SDL_GetTicks() - pauseStartTime;
    } else if (isMouseOverButton(mouseX, mouseY, homeButton)) {
        currentState = GameState::MENU;
        level = 1;
        score = 0;
    } else if (isMouseOverButton(mouseX, mouseY, replayButton)) {
        currentLevelScore = 0;
        score = previousScore;
        currentState = GameState::PLAYING;
        levelStartTime = SDL_GetTicks();
    }
}

// Cleanup
void close() {
    if (score > highestScore) {
        writeHighestScore(score);
    }
    for (auto texture : emojiTextures) SDL_DestroyTexture(texture);
    SDL_DestroyTexture(backTexture);
    SDL_DestroyTexture(matchedTexture);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(menuTexture);
    SDL_DestroyTexture(infoTexture);
    SDL_DestroyTexture(soundOffTexture);
    SDL_DestroyTexture(pauseWindowTexture);
    SDL_DestroyTexture(gameOverWindowTexture);

    Mix_FreeChunk(flipSound);
    Mix_FreeChunk(matchedSound);
    Mix_FreeChunk(wrongSound);
    Mix_FreeMusic(bgMusic);

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    Mix_CloseAudio();
    Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

// Main game loop
int main(int argc, char* argv[]) {
    if (!init() || !loadMedia()) {
        close();
        return -1;
    }

    highestScore = readHighestScore();

    const SDL_Color brown = {39, 32, 22, 255};
    bool quit = false;
    SDL_Event e;

    while (!quit) {
        if (isWaiting && SDL_GetTicks() - flipTime > FLIP_DELAY_MS) {
            for (auto* card : flippedCards) card->isFaceUp = false;
            flippedCards.clear();
            isWaiting = false;
        }

        if (isLastPairMatched && SDL_GetTicks() - lastPairTime > LAST_PAIR_DELAY_MS) {
            isLastPairMatched = false;
            if (level < MAX_LEVEL) {
                checkAndUpdateHighScore();
                level++;
                initLevel();
            } else {
                checkAndUpdateHighScore();
                currentState = GameState::GAME_OVER;
            }
        }

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);

                switch (currentState) {
                    case GameState::MENU: handleMenuClick(mouseX, mouseY); break;
                    case GameState::PLAYING:
                        if (isMouseOverButton(mouseX, mouseY, pauseButton)) {
                            currentState = GameState::PAUSED;
                            pauseStartTime = SDL_GetTicks();
                        } else {
                            handleCardClick(mouseX, mouseY);
                        }
                        break;
                    case GameState::PAUSED: handlePauseClick(mouseX, mouseY); break;
                    case GameState::GAME_OVER: {
                        SDL_Point mousePoint = {mouseX, mouseY};
                        if (SDL_PointInRect(&mousePoint, &homeButton.rect)) {
                            currentState = GameState::MENU;
                            level = 1;
                            score = 0;
                        } else if (SDL_PointInRect(&mousePoint, &replayButton.rect)) {
                            currentState = GameState::PLAYING;
                            initLevel();
                            score = 0;
                        }
                        break;
                    }
                    default: break;
                }
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                if (currentState == GameState::INFO || currentState == GameState::PAUSED) {
                    currentState = GameState::MENU;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
        SDL_RenderClear(renderer);

        if (backgroundTexture && currentState != GameState::MENU && currentState != GameState::INFO) {
            SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);
        }

        switch (currentState) {
            case GameState::MENU: renderMenu(); break;
            case GameState::PLAYING:
                renderCards();
                renderTimeBar();
                renderText("Level: " + to_string(level), 20, SCREEN_HEIGHT - 56, brown);
                renderText("Score: " + to_string(score), SCREEN_WIDTH - 200, 25, brown);
                renderText("Highest Score: " + to_string(highestScore), 600, SCREEN_HEIGHT - 56, brown);
                break;
            case GameState::GAME_OVER:
                renderCards();
                checkAndUpdateHighScore();
                renderText("Level: " + to_string(level), 20, SCREEN_HEIGHT - 56, brown);
                renderText("Score: " + to_string(score), SCREEN_WIDTH - 200, 25, brown);
                renderText("Highest Score: " + to_string(highestScore), 600, SCREEN_HEIGHT - 56, brown);
                renderGameOverScreen();
                break;
            case GameState::INFO: renderInfoScreen(); break;
            case GameState::PAUSED:
                renderCards();
                renderTimeBar();
                renderText("Level: " + to_string(level), 20, SCREEN_HEIGHT - 56, brown);
                renderText("Score: " + to_string(score), SCREEN_WIDTH - 200, 25, brown);
                renderText("Highest Score: " + to_string(highestScore), 600, SCREEN_HEIGHT - 56, brown);
                renderPauseScreen();
                break;
        }

        SDL_RenderPresent(renderer);
    }

    close();
    return 0;
}

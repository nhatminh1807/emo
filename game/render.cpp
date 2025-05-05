#include "render.h"
#include "global.h"
#include "audio.h"
#include "logic.h"



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
        playSound(loseSound);
        checkAndUpdateHighScore();
    }

    const int TIME_BAR_WIDTH = 470;
    const int TIME_BAR_HEIGHT = 20;
    const int TIME_BAR_X = (SCREEN_WIDTH - TIME_BAR_WIDTH) / 2;
    const int TIME_BAR_Y = 15;

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
    renderText("Final Score: " + to_string(score),
              windowRect.x + (windowWidth - 200) / 2 - 30,
              windowRect.y + windowRect.h / 2 - 55,
              brown);
    renderText("HIGHEST SCORE: " + to_string(highestScore),
              windowRect.x + (windowWidth - 200) / 2 - 60,
              windowRect.y + windowRect.h / 2 - 100,
              brown);
}

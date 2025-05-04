#include "game_globals.h"
#include "game_types.h"
#include "game_logic.h"
#include "asset.h"
#include "audio.h"
#include "render.h"
#include "input.h"

int main(int argc, char* argv[]) {

    if (!initSDL() || !loadMedia()) return -1;

    highestScore = readHighestScore();
    score = 0;

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
                            playClickSound();
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
                            playClickSound();
                            checkAndUpdateHighScore();
                            currentState = GameState::MENU;
                            level = 1;
                            score = 0;
                        } else if (SDL_PointInRect(&mousePoint, &replayButton.rect)) {
                            playClickSound();
                            checkAndUpdateHighScore();
                            level = 1;
                            score = 0;
                            currentState = GameState::PLAYING;
                            initLevel();
                        }
                        break;
                    }
                    default: break;
                }
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                if (currentState == GameState::PLAYING) {
                    currentState = GameState::PAUSED;
                    pauseStartTime = SDL_GetTicks();
                } else if (currentState == GameState::PAUSED) {
                    currentState = GameState::PLAYING;
                    levelStartTime += SDL_GetTicks() - pauseStartTime;
                } else if (currentState == GameState::INFO) {
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
                renderGameOverScreen();
                renderText("Level: " + to_string(level), 20, SCREEN_HEIGHT - 56, brown);
                renderText("Score: " + to_string(score), SCREEN_WIDTH - 200, 25, brown);
                renderText("Highest Score: " + to_string(highestScore), 600, SCREEN_HEIGHT - 56, brown);
                break;
            case GameState::INFO: renderInfoScreen(); break;
            case GameState::PAUSED:
                renderCards();
                renderPauseScreen();
                renderTimeBar();
                renderText("Level: " + to_string(level), 20, SCREEN_HEIGHT - 56, brown);
                renderText("Score: " + to_string(score), SCREEN_WIDTH - 200, 25, brown);
                renderText("Highest Score: " + to_string(highestScore), 600, SCREEN_HEIGHT - 56, brown);
                break;
        }

        SDL_RenderPresent(renderer);
    }

    close();

    return 0;
}

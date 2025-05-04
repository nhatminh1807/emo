#include "input.h"
#include "game_globals.h"
#include "audio.h"
#include "game_logic.h"
#include <SDL.h>

bool isMouseOverButton(int mouseX, int mouseY, const Button& button) {
    return (mouseX >= button.rect.x) &&
           (mouseX <= button.rect.x + button.rect.w) &&
           (mouseY >= button.rect.y) &&
           (mouseY <= button.rect.y + button.rect.h);
}

void handleMenuClick(int mouseX, int mouseY) {
    if (isMouseOverButton(mouseX, mouseY, startButton)) {
        playClickSound();
        currentState = GameState::PLAYING;
        initLevel();
    } else if (isMouseOverButton(mouseX, mouseY, infoButton)) {
        playClickSound();
        currentState = GameState::INFO;
    } else if (isMouseOverButton(mouseX, mouseY, soundButton)) {
        playClickSound();
        isSoundOn = !isSoundOn;
        if (isSoundOn) Mix_ResumeMusic();
        else Mix_PauseMusic();
    }
    else if (isMouseOverButton(mouseX, mouseY, exitButton)) {
        playClickSound();
        SDL_Event quitEvent;
        quitEvent.type = SDL_QUIT;
        SDL_PushEvent(&quitEvent);
    }
}

void handleCardClick(int mouseX, int mouseY) {
    if (isWaiting || currentState != GameState::PLAYING || isTimeUp || isLastPairMatched) return;

    for (auto& card : cards) {
        SDL_Point mousePos = {mouseX, mouseY};
        if (!card.isFaceUp && !card.isMatched && SDL_PointInRect(&mousePos, &card.rect)) {
            card.isFaceUp = true;
            flippedCards.push_back(&card);
            playSound(flipSound);

            if (flippedCards.size() == 2) {
                int v1 = flippedCards[0]->value;
                int v2 = flippedCards[1]->value;

                if (v1 == v2) {
                    if (v1 == BOMB_CARD_VALUE) {
                    playSound(loseSound);
                    isTimeUp = true;
                    currentState = GameState::GAME_OVER;
                    flippedCards.clear();
                }
                else {
                    flippedCards[0]->isMatched = true;
                    flippedCards[1]->isMatched = true;
                    pairsFound++;

                    if (v1 == X2_CARD_VALUE) {
                        playSound(x2Sound);
                        currentLevelScore += (10 * level) * 2;
                        score += (10 * level) * 2;
                    } else {
                        playSound(matchedSound);
                        currentLevelScore += 10 * level;
                        score += 10 * level;
                    }

                    flippedCards.clear();

                    if (pairsFound == normalPairs) {
                        isLastPairMatched = true;
                        lastPairTime = SDL_GetTicks();
                    }
                }
            }
            else {
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
        playClickSound();
        currentState = GameState::PLAYING;
        levelStartTime += SDL_GetTicks() - pauseStartTime;
    } else if (isMouseOverButton(mouseX, mouseY, homeButton)) {
        playClickSound();
        currentState = GameState::MENU;
        level = 1;
        score = 0;
    } else if (isMouseOverButton(mouseX, mouseY, replayButton)) {
        playClickSound();
        currentLevelScore = 0;
        score = previousScore;
        initLevel();
        currentState = GameState::PLAYING;
        levelStartTime = SDL_GetTicks();
    }
}


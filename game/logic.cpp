#include "logic.h"
#include "global.h"
#include "types.h"

bool initLevel() {
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

    normalPairs = 0;
    for (int v : values) {
        if (v != BOMB_CARD_VALUE) {
            normalPairs++;
        }
    }
    normalPairs /= 2;


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

    return true;
}

int readHighestScore() {
    ifstream file("highscore.txt");
    int hs = 0;
    if (file.is_open()) {
        file >> hs;
        file.close();
    }
    return hs;
}

void checkAndUpdateHighScore() {
    if (score > highestScore) {
        highestScore = score;
        ofstream file("highscore.txt");
        if (file.is_open()) {
            file << highestScore;
            file.close();
        }
    }
}



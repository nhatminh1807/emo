#include "asset.h"
#include "game_globals.h"
#include "audio.h"
#include <SDL_image.h>

bool initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "SDL initialization failed: " << SDL_GetError() << endl;
        return false;
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

    window = SDL_CreateWindow("Emoji Matching Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return false;

    if (TTF_Init() == -1 || !(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) return false;

    font = TTF_OpenFont("fredoka.ttf", 36);
    if (!font) return false;

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) return false;

    srand(SDL_GetTicks());

    return true;
}


SDL_Texture* loadTexture(const string& path) {
    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) return nullptr;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

bool loadMedia() {
    const vector<string> emojiFiles = {
        "emojis/smile.png", "emojis/heart.png", "emojis/bomb.png", "emojis/x2.png", "emojis/cat.png",
        "emojis/dog.png", "emojis/sun.png", "emojis/moon.png", "emojis/star.png", "emojis/tree.png"
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
    clickSound = Mix_LoadWAV("sounds/click.wav");
    loseSound = Mix_LoadWAV("sounds/lose.wav");
    x2Sound = Mix_LoadWAV("sounds/x2.wav");

    if (!flipSound || !matchedSound || !wrongSound || !bgMusic || !clickSound || !loseSound || !x2Sound) {
        cerr << "Failed to load sound files! Error: " << Mix_GetError() << endl;
        return false;
    }

    if (isSoundOn) {
        Mix_PlayMusic(bgMusic, -1);
        Mix_VolumeMusic(5);
    }

    return true;
}

void close() {
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
    Mix_FreeChunk(clickSound);
    Mix_FreeChunk(loseSound);
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


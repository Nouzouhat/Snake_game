#include <stdio.h>
#include <SDL2/SDL.h>

#include <stdbool.h>
#include <time.h>


#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SNAKE_SIZE 20
#define APPLE_SIZE 20
#define MAX_LENGTH 100

typedef struct {
    int x, y;
} Segment;

Segment snake[MAX_LENGTH];
int snakeLength = 1;
int foodX, foodY;
int applesEaten = 0;
int directionX = 1, directionY = 0;
bool gameover = false;

SDL_Window* window;
SDL_Renderer* renderer;

void spawnFood() {
    foodX = (rand() % (SCREEN_WIDTH / APPLE_SIZE)) * APPLE_SIZE;
    foodY = (rand() % (SCREEN_HEIGHT / APPLE_SIZE)) * APPLE_SIZE;
}

void moveSnake() {
    int nextX = snake[0].x + directionX * SNAKE_SIZE;
    int nextY = snake[0].y + directionY * SNAKE_SIZE;

    // Déplacer le serpent en ajoutant un nouveau segment à la tête
    for (int i = snakeLength; i > 0; i--) {
        snake[i] = snake[i - 1];
    }

    snake[0].x = nextX;
    snake[0].y = nextY;
}

bool checkCollision() {
    // Vérifier les collisions avec les bords de l'écran
    if (snake[0].x < 0 || snake[0].x >= SCREEN_WIDTH || snake[0].y < 0 || snake[0].y >= SCREEN_HEIGHT) {
        return true;
    }

    // Vérifier les collisions avec le corps du serpent
    for (int i = 1; i < snakeLength; i++) {
        if (snake[i].x == snake[0].x && snake[i].y == snake[0].y) {
            return true;
        }
    }

    return false;
}

void saveGameToFile() {
    // Ouvrir un fichier en mode écriture pour sauvegarder l'état du jeu
    FILE* saveFile = fopen("snake_save.txt", "w");
    if (saveFile) {
        fprintf(saveFile, "%d\n", snakeLength);
        for (int i = 0; i < snakeLength; i++) {
            fprintf(saveFile, "%d %d\n", snake[i].x, snake[i].y);
        }
        fprintf(saveFile, "%d %d\n", foodX, foodY);
        fprintf(saveFile, "%d\n", applesEaten);
        fclose(saveFile);
        printf("Partie sauvegardée. Appuyez sur 'L' pour charger la partie.\n");
    } else {
        printf("Échec de la sauvegarde de la partie.\n");
    }
}

void loadGameFromFile() {
    // Ouvrir un fichier en mode lecture pour charger l'état du jeu
    FILE* saveFile = fopen("snake_save.txt", "r");
    if (saveFile) {
        fscanf(saveFile, "%d", &snakeLength);
        for (int i = 0; i < snakeLength; i++) {
            fscanf(saveFile, "%d %d", &snake[i].x, &snake[i].y);
        }
        fscanf(saveFile, "%d %d", &foodX, &foodY);
        fscanf(saveFile, "%d", &applesEaten);
        fclose(saveFile);
        printf("Partie chargée. Appuyez sur 'S' pour sauvegarder la partie.\n");
    } else {
        printf("Aucune partie sauvegardée trouvée.\n");
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Échec de l'initialisation de SDL : %s\n", SDL_GetError());
        return 1;
    }

    window = SDL_CreateWindow("Jeu de serpent", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    srand(time(NULL));
    snake[0].x = SCREEN_WIDTH / 2;
    snake[0].y = SCREEN_HEIGHT / 2;

    // Charger la partie sauvegardée s'il y en a une
    loadGameFromFile();

    // Spawn initial de la nourriture
    spawnFood();

    while (!gameover) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        if (directionY != 1) {
                            directionX = 0;
                            directionY = -1;
                        }
                        break;
                    case SDLK_DOWN:
                        if (directionY != -1) {
                            directionX = 0;
                            directionY = 1;
                        }
                        break;
                    case SDLK_LEFT:
                        if (directionX != 1) {
                            directionX = -1;
                            directionY = 0;
                        }
                        break;
                    case SDLK_RIGHT:
                        if (directionX != -1) {
                            directionX = 1;
                            directionY = 0;
                        }
                        break;
                    case SDLK_s: // Touche "S" pour sauvegarder
                        saveGameToFile();
                        break;
                    case SDLK_l: // Touche "L" pour charger
                        loadGameFromFile();
                        break;
                }
            }
        }

        moveSnake();

        if (snake[0].x == foodX && snake[0].y == foodY) {
            applesEaten++;
            if (applesEaten % 3 == 0) {
                snakeLength++;
            }
            spawnFood();
        }

        if (checkCollision()) {
            gameover = true;
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        for (int i = 0; i < snakeLength; i++) {
            SDL_Rect rect = {snake[i].x, snake[i].y, SNAKE_SIZE, SNAKE_SIZE};
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderFillRect(renderer, &rect);
        }

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect foodRect = {foodX, foodY, APPLE_SIZE, APPLE_SIZE};
        SDL_RenderFillRect(renderer, &foodRect);

        SDL_RenderPresent(renderer);

        SDL_Delay(100); // Délai pour contrôler la vitesse du jeu
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

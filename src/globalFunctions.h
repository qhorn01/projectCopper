#ifndef GLOBAL_FUNCTIONS_H
#define GLOBAL_FUNCTIONS_H

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "raylib.h"

typedef enum { // defined in main.c
    MENU,
    TEST_CENTER // for debugging and testing only
} GameState; // changes different testCenter ex. main menu, rooms, levels, minigames, etc.

typedef struct {
    Vector2 position; // x and y position of Character
    Vector2 spriteIndex; // x, y index on sprite sheet
    int width; // width of Character
    int height; // height of Character
    int health; // health of Character
    int moveFrameCount; // number of frames for Character's movement animation
    int moveFrameCurrent; // current frame of Character's movement animation
    float speed; // speed of Character
    float animTimerMax; // maximum time for animation frame
} Character; // item that can be dragged across the screen with the mouse and dropped at a specific location on the screen

void renderTileMap(int *map, int mapHeight, int mapWidth, float tileWidth, float tileHeight, Texture2D tileset);
void renderCharacter(Character *Character, Texture2D spriteSheet);
void updateCamera(Camera2D *camera, Character *target, int screenWidth, int screenHeight, int mapWidth, int mapHeight, float tileWidth, float tileHeight);

bool checkTileCollision(Rectangle hitbox, int *map, int mapHeight, int mapWidth, float tileWidth, float tileHeight, const int *solidTiles, int solidTileCount);
Rectangle getCharacterHitbox(Character *character);
bool checkCharacterCollision(Rectangle hitbox, Character *self, Character *characters[], int characterCount);
void moveCharacter(Character *character, int dirX, int dirY, int *map, int mapHeight, int mapWidth, float tileWidth, float tileHeight, const int *solidTiles, int solidTileCount, Character *characters[], int characterCount);
void movePlayer(Character *player, int *map, int mapHeight, int mapWidth, float tileWidth, float tileHeight, const int *solidTiles, int solidTileCount, Character *characters[], int characterCount);

#endif
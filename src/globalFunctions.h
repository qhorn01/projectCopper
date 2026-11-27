#ifndef GLOBAL_FUNCTIONS_H
#define GLOBAL_FUNCTIONS_H

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

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
    float frame; // counts the number of frames counted
    float frameReset; // determines how many frames have to pass before it resets
} Character; // item that can be dragged across the screen with the mouse and dropped at a specific location on the screen

void renderTileMap(int *map, int mapHeight, int mapWidth, float tileWidth, float tileHeight, Texture2D tileset);
void renderCharacter(Character *Character, Texture2D spriteSheet);

#endif
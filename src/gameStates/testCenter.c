#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#include "raylib.h"
#include "../globalFunctions.h"

// structs
Character player = {
    .position = { 16.0f, 16.0f },
    .width = 16.0f,
    .height = 16.0f,
    .spriteIndex = { 0.0f, 0.0f },
    .health = 10,
    .moveFrameCount = 4,
    .moveFrameCurrent = 1,
    .speed = 2.0f,
    .animTimerMax = 0.15f
};

Character npc1 = {
    .position = { 16.0f, 160.0f },
    .width = 16.0f,
    .height = 16.0f,
    .spriteIndex = { 0.0f, 0.0f },
    .health = 10,
    .moveFrameCount = 4,
    .moveFrameCurrent = 1,
    .speed = 2.0f,
    .animTimerMax = 0.15f
};

// List of all active characters in this level for character-to-character collision
Character *testCenterCharacters[] = { &player, &npc1 };
const int testCenterCharacterCount = sizeof(testCenterCharacters) / sizeof(testCenterCharacters[0]);

static Camera2D camera = { 0 };

// tile textures
static Texture2D testCenterTileset;
// sprite textures
static Texture2D testCenterSpriteSheet;
static Texture2D testCenterSpriteSheet2;

void initTestCenter(void){
    testCenterTileset = LoadTexture("assets/tiles/testCenter/testTiles.png");
    testCenterSpriteSheet = LoadTexture("assets/tiles/testCenter/testCenterSpriteSheet.png");
    testCenterSpriteSheet2 = LoadTexture("assets/tiles/testCenter/testCenterSpriteSheet2.png");
}

void unloadTestCenter(void){
    UnloadTexture(testCenterTileset);
    UnloadTexture(testCenterSpriteSheet);
    UnloadTexture(testCenterSpriteSheet2);
}

#define MAP_WIDTH  32
#define MAP_HEIGHT 36

int testCenterTileMap[MAP_WIDTH * MAP_HEIGHT] = {
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 12, 12, 12, 12, 12, 12, 13, 12, 13, 12, 12, 13, 13, 12, 12, 12, 12, 12,
    12,  2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2, 12,
    12,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4, 12,
    12,  5,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  4, 12,
    12,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4, 13,
    13,  5,  0,  0,  0, 11,  6,  6,  6,  6,  6,  6,  6,  6, 10,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4, 13,
    12,  5,  0,  0,  0,  4, 15, 14, 14, 15, 15, 14, 14, 14,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4, 13,
    12,  5,  0,  0,  0,  4, 14, 12, 12, 14, 14, 13, 12, 15,  5,  0, 11,  6,  6,  6,  6,  6,  6,  6,  6, 10,  0,  0,  0,  0,  4, 13,
    13,  5,  0,  0,  0,  4, 14, 14, 15, 14, 15, 15, 14, 14,  5,  0,  4, 15, 14, 14, 15, 15, 14, 14, 14,  5,  0,  0,  0,  0,  4, 12,
    13,  5,  0,  0,  0,  8,  7,  7,  7,  7,  7,  7,  7,  7,  9,  0,  4, 14, 13, 12, 14, 14, 13, 12, 15,  5,  0,  0,  0,  0,  4, 13,
    13,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4, 14, 14, 15, 14, 15, 15, 14, 14,  5,  0,  0,  0,  0,  4, 12,
    12,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  8,  7,  7,  7,  7,  7,  7,  7,  7,  9,  0,  0,  0,  0,  4, 13,
    12,  5,  0,  0,  0,  3,  2,  3,  2,  3,  3,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4, 12,
    12,  5,  0,  0,  0,  3,  3,  3,  2,  2,  3,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4, 12,
    12,  5,  0,  0,  0,  3,  2,  3,  3,  3,  3,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  4, 12,
    12,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4, 13,
    12,  2,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  2, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 12, 12, 14, 14, 15, 15, 13, 12, 13, 12, 12, 13, 13, 12, 12, 12, 12, 12, 12, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 15, 15, 14, 15, 12, 12, 13, 12, 13, 12, 12, 13, 13, 12, 12, 12, 12, 12,
    12,  2,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  2, 12,
    12,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4, 12,
    12,  5,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  4, 12,
    12,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4, 13,
    13,  5,  0,  0,  0, 11,  6,  6,  6,  6,  6,  6,  6,  6, 10,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4, 13,
    12,  5,  0,  0,  0,  4, 15, 14, 14, 15, 15, 14, 14, 14,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4, 13,
    12,  5,  0,  0,  0,  4, 14, 12, 12, 14, 14, 13, 12, 15,  5,  0, 11,  6,  6,  6,  6,  6,  6,  6,  6, 10,  0,  0,  0,  0,  4, 13,
    13,  5,  0,  0,  0,  4, 14, 14, 15, 14, 15, 15, 14, 14,  5,  0,  4, 15, 14, 14, 15, 15, 14, 14, 14,  5,  0,  0,  0,  0,  4, 12,
    13,  5,  0,  0,  0,  8,  7,  7,  7,  7,  7,  7,  7,  7,  9,  0,  4, 14, 13, 12, 14, 14, 13, 12, 15,  5,  0,  0,  0,  0,  4, 13,
    13,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4, 14, 14, 15, 14, 15, 15, 14, 14,  5,  0,  0,  0,  0,  4, 12,
    12,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  8,  7,  7,  7,  7,  7,  7,  7,  7,  9,  0,  0,  0,  0,  4, 13,
    12,  5,  0,  0,  0,  3,  2,  3,  2,  3,  3,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4, 12,
    12,  5,  0,  0,  0,  3,  3,  3,  2,  2,  3,  3,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4, 12,
    12,  5,  0,  0,  0,  3,  2,  3,  3,  3,  3,  2,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  4, 12,
    12,  5,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4, 13,
    12,  2,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  2, 12,
    12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 12, 12, 12, 12, 12, 12, 13, 12, 13, 12, 12, 13, 13, 12, 12, 12, 12, 12, 12, 12
};

// Solid tile IDs specific to this map
const int testCenterSolidTiles[] = { 12, 13 };
const int testCenterSolidCount = sizeof(testCenterSolidTiles) / sizeof(testCenterSolidTiles[0]);

void testCenterLogic(GameState *currentState, Vector2 mouse){
    movePlayer(&player, testCenterTileMap, MAP_HEIGHT, MAP_WIDTH, 8.0f, 8.0f, testCenterSolidTiles, testCenterSolidCount, testCenterCharacters, testCenterCharacterCount);

    // Later, to make npc1 move with full wall + player collision, just call:
    // moveCharacter(&npc1, npcDirX, npcDirY, testCenterTileMap, MAP_HEIGHT, MAP_WIDTH, 8.0f, 8.0f, testCenterSolidTiles, testCenterSolidCount, testCenterCharacters, testCenterCharacterCount);

    updateCamera(&camera, &player, 256, 144, MAP_WIDTH, MAP_HEIGHT, 8.0f, 8.0f);
}

void testCenterRender(){
    BeginMode2D(camera);
        renderTileMap(testCenterTileMap, MAP_HEIGHT, MAP_WIDTH, 8.0f, 8.0f, testCenterTileset);

        // Y-sort so whichever character is lower on screen renders in front
        if (npc1.position.y < player.position.y) {
            renderCharacter(&npc1, testCenterSpriteSheet2);
            renderCharacter(&player, testCenterSpriteSheet);
        } else {
            renderCharacter(&player, testCenterSpriteSheet);
            renderCharacter(&npc1, testCenterSpriteSheet2);
        }
    EndMode2D();
}
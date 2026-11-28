#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "raylib.h"
#include "globalFunctions.h"
/*
This .c file contains functions that correspond with specific structs
Everytime you see something like "Item struct:" that indicates that all
functions below it are to only be used for that specified struct
*/

void renderTileMap(int *map, int mapHeight, int mapWidth, float tileWidth, float tileHeight, Texture2D tileset) {
    // Prevent division-by-zero crash if texture failed to load or tile dimensions are invalid
    if (tileset.id <= 0 || tileWidth <= 0.0f || tileHeight <= 0.0f) return;

    // Calculate how many tiles fit across one row of your PNG
    int columns = tileset.width / (int)tileWidth;
    if (columns <= 0) return;

    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            
            int tileIndex = map[(y * mapWidth) + x];

            // Skip empty cells if you leave any blank spaces in Tiled (-1)
            if (tileIndex < 0) continue;

            // Calculate both X (column) and Y (row) on the grid sheet
            Rectangle sourceRec = { 
                (float)((tileIndex % columns) * tileWidth),  // X moves right across columns
                (float)((tileIndex / columns) * tileHeight), // Y moves down each time a row finishes
                tileWidth,
                tileHeight
            };

            Vector2 position = { x * tileWidth, y * tileHeight };

            DrawTextureRec(tileset, sourceRec, position, WHITE);
        }
    }
}

void renderCharacter(Character *character, Texture2D spriteSheet){ // renders Character to screen
    if (spriteSheet.id <= 0) return; // Prevent division-by-zero crash if texture failed to load
    DrawTextureRec(spriteSheet, 
                    (Rectangle){ 
                        character->spriteIndex.x * character->width, 
                        character->spriteIndex.y * character->height,
                        character->width, 
                        character->height },
                        (Vector2){ roundf(character->position.x), roundf(character->position.y) }, 
                        WHITE);
}

void updateCamera(Camera2D *camera, Character *target, int screenWidth, int screenHeight, int mapWidth, int mapHeight, float tileWidth, float tileHeight) {
    // Center the camera on the middle of the character sprite
    float targetX = target->position.x + (target->width * 0.5f);
    float targetY = target->position.y + (target->height * 0.5f);

    float halfScreenW = screenWidth * 0.5f;
    float halfScreenH = screenHeight * 0.5f;

    float mapPixelW = mapWidth * tileWidth;
    float mapPixelH = mapHeight * tileHeight;

    // Clamp camera X so it doesn't show black space past the left/right edges of the map
    if (mapPixelW <= screenWidth) {
        targetX = mapPixelW * 0.5f;
    } else {
        if (targetX < halfScreenW) targetX = halfScreenW;
        if (targetX > mapPixelW - halfScreenW) targetX = mapPixelW - halfScreenW;
    }

    // Clamp camera Y so it doesn't show black space past the top/bottom edges of the map
    if (mapPixelH <= screenHeight) {
        targetY = mapPixelH * 0.5f;
    } else {
        if (targetY < halfScreenH) targetY = halfScreenH;
        if (targetY > mapPixelH - halfScreenH) targetY = mapPixelH - halfScreenH;
    }

    // Round to whole pixels to prevent tile seams and sprite shimmer
    camera->target = (Vector2){ roundf(targetX), roundf(targetY) };
    camera->offset = (Vector2){ halfScreenW, halfScreenH };
    camera->rotation = 0.0f;
    camera->zoom = 1.0f;
}

Rectangle getCharacterHitbox(Character *character) {
    // Feet hitbox (10x6 at the bottom center of the 16x16 sprite)
    return (Rectangle){
        character->position.x + 3.0f,
        character->position.y + 10.0f,
        10.0f,
        6.0f
    };
}

bool checkTileCollision(Rectangle hitbox, int *map, int mapHeight, int mapWidth, float tileWidth, float tileHeight, const int *solidTiles, int solidTileCount) {
    if (map == NULL || solidTiles == NULL || solidTileCount <= 0) return false;

    // Convert hitbox edges into tile grid coordinates (subtract small epsilon on right/bottom edges)
    int leftTile   = (int)floorf(hitbox.x / tileWidth);
    int rightTile  = (int)floorf((hitbox.x + hitbox.width - 0.01f) / tileWidth);
    int topTile    = (int)floorf(hitbox.y / tileHeight);
    int bottomTile = (int)floorf((hitbox.y + hitbox.height - 0.01f) / tileHeight);

    // Treat out-of-bounds map edges as solid
    if (leftTile < 0 || rightTile >= mapWidth || topTile < 0 || bottomTile >= mapHeight) {
        return true;
    }

    // Check every tile overlapped by the hitbox
    for (int y = topTile; y <= bottomTile; y++) {
        for (int x = leftTile; x <= rightTile; x++) {
            int tileID = map[(y * mapWidth) + x];

            for (int i = 0; i < solidTileCount; i++) {
                if (tileID == solidTiles[i]) {
                    return true;
                }
            }
        }
    }

    return false;
}

bool checkCharacterCollision(Rectangle hitbox, Character *self, Character *characters[], int characterCount) {
    if (characters == NULL || characterCount <= 0) return false;

    for (int i = 0; i < characterCount; i++) {
        Character *other = characters[i];
        // Skip null entries and skip checking collision against itself
        if (other == NULL || other == self) continue;

        Rectangle otherHitbox = getCharacterHitbox(other);
        if (CheckCollisionRecs(hitbox, otherHitbox)) {
            return true;
        }
    }

    return false;
}

void moveCharacter(Character *character, int dirX, int dirY, int *map, int mapHeight, int mapWidth, float tileWidth, float tileHeight, const int *solidTiles, int solidTileCount, Character *characters[], int characterCount) {
    float dt = GetFrameTime();
    float moveSpeed = character->speed * 30.0f; // Scales speed = 2.0f to 60 px/sec

    if (dirX != 0 || dirY != 0) {
        // Normalize diagonal movement so diagonal speed matches cardinal speed
        float len = sqrtf((float)(dirX * dirX + dirY * dirY));
        float velX = (dirX / len) * moveSpeed * dt;
        float velY = (dirY / len) * moveSpeed * dt;

        // 1. Move and check X axis against both solid tiles and other characters
        float nextX = character->position.x + velX;
        Rectangle hitboxX = { nextX + 3.0f, character->position.y + 10.0f, 10.0f, 6.0f };
        if (!checkTileCollision(hitboxX, map, mapHeight, mapWidth, tileWidth, tileHeight, solidTiles, solidTileCount) &&
            !checkCharacterCollision(hitboxX, character, characters, characterCount)) {
            character->position.x = nextX;
        }

        // 2. Move and check Y axis against both solid tiles and other characters
        float nextY = character->position.y + velY;
        Rectangle hitboxY = { character->position.x + 3.0f, nextY + 10.0f, 10.0f, 6.0f };
        if (!checkTileCollision(hitboxY, map, mapHeight, mapWidth, tileWidth, tileHeight, solidTiles, solidTileCount) &&
            !checkCharacterCollision(hitboxY, character, characters, characterCount)) {
            character->position.y = nextY;
        }

        // Check vertical first so diagonal movement displays Up (1) or Down (0)
        if (dirY > 0)      character->spriteIndex.x = 0.0f; // Down
        else if (dirY < 0) character->spriteIndex.x = 1.0f; // Up
        else if (dirX < 0) character->spriteIndex.x = 2.0f; // Left
        else if (dirX > 0) character->spriteIndex.x = 3.0f; // Right

        // Note: if you want per-character animation timers later, add `float animTimer` to Character
        static float animTimer = 0.0f;
        animTimer += dt;
        if (animTimer >= character->animTimerMax) {
            animTimer = 0.0f;
            character->moveFrameCurrent += 1;
            if (character->moveFrameCurrent >= character->moveFrameCount) character->moveFrameCurrent = 1;
        }
        character->spriteIndex.y = (float)character->moveFrameCurrent;
    } else {
        // Idle standing frame (row 0) while keeping last faced direction
        character->spriteIndex.y = 0.0f;
    }
}

void movePlayer(Character *player, int *map, int mapHeight, int mapWidth, float tileWidth, float tileHeight, const int *solidTiles, int solidTileCount, Character *characters[], int characterCount) {
    int dirX = 0;
    int dirY = 0;

    if (IsKeyDown(KEY_A)) dirX -= 1;
    if (IsKeyDown(KEY_D)) dirX += 1;
    if (IsKeyDown(KEY_W)) dirY -= 1;
    if (IsKeyDown(KEY_S)) dirY += 1;

    moveCharacter(player, dirX, dirY, map, mapHeight, mapWidth, tileWidth, tileHeight, solidTiles, solidTileCount, characters, characterCount);
}
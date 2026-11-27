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

void renderCharacter(Character *Character, Texture2D spriteSheet){ // renders Character to screen
    if (spriteSheet.id <= 0) return; // Prevent division-by-zero crash if texture failed to load
    DrawTextureRec(spriteSheet, 
                    (Rectangle){ 
                        Character->spriteIndex.x * Character->width, 
                        Character->spriteIndex.y * Character->height,
                        Character->width, 
                        Character->height },
                        (Vector2){ roundf(Character->position.x), roundf(Character->position.y) }, 
                        WHITE);
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

void movePlayer(Character *player, int *map, int mapHeight, int mapWidth, float tileWidth, float tileHeight, const int *solidTiles, int solidTileCount) {
    static float animTimer = 0.0f;

    float dt = GetFrameTime();
    float moveSpeed = player->speed * 30.0f; // Scales speed = 2.0f to 60 px/sec

    int dirX = 0;
    int dirY = 0;

    if (IsKeyDown(KEY_A)) dirX -= 1;
    if (IsKeyDown(KEY_D)) dirX += 1;
    if (IsKeyDown(KEY_W)) dirY -= 1;
    if (IsKeyDown(KEY_S)) dirY += 1;

    if (dirX != 0 || dirY != 0) {
        // Normalize diagonal movement so diagonal speed matches cardinal speed
        float len = sqrtf((float)(dirX * dirX + dirY * dirY));
        float velX = (dirX / len) * moveSpeed * dt;
        float velY = (dirY / len) * moveSpeed * dt;

        // Feet hitbox (10x6 at the bottom center of the 16x16 sprite)
        float hitboxOffsetX = 3.0f;
        float hitboxOffsetY = 10.0f;
        float hitboxWidth   = 10.0f;
        float hitboxHeight  = 6.0f;

        // 1. Move and check X axis independently (allows smooth sliding along walls)
        float nextX = player->position.x + velX;
        Rectangle hitboxX = { nextX + hitboxOffsetX, player->position.y + hitboxOffsetY, hitboxWidth, hitboxHeight };
        if (!checkTileCollision(hitboxX, map, mapHeight, mapWidth, tileWidth, tileHeight, solidTiles, solidTileCount)) {
            player->position.x = nextX;
        }

        // 2. Move and check Y axis independently
        float nextY = player->position.y + velY;
        Rectangle hitboxY = { player->position.x + hitboxOffsetX, nextY + hitboxOffsetY, hitboxWidth, hitboxHeight };
        if (!checkTileCollision(hitboxY, map, mapHeight, mapWidth, tileWidth, tileHeight, solidTiles, solidTileCount)) {
            player->position.y = nextY;
        }

        // Check vertical first so diagonal movement displays Up (1) or Down (0)
        if (dirY > 0)      player->spriteIndex.x = 0.0f; // Down
        else if (dirY < 0) player->spriteIndex.x = 1.0f; // Up
        else if (dirX < 0) player->spriteIndex.x = 2.0f; // Left
        else if (dirX > 0) player->spriteIndex.x = 3.0f; // Right

        // Cycle between walking frames
        animTimer += dt;
        if (animTimer >= player->animTimerMax) {
            animTimer = 0.0f;
            player->moveFrameCurrent += 1;
            if (player->moveFrameCurrent >= player->moveFrameCount) player->moveFrameCurrent = 1;
        }
        player->spriteIndex.y = (float)player->moveFrameCurrent;
    } else {
        // Idle standing frame (row 0) while keeping last faced direction
        animTimer = 0.0f;
        player->spriteIndex.y = 0.0f;
    }
}
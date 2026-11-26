#include <math.h>
#include <stdint.h>
#include <stdlib.h>

#include "raylib.h"
#include "globalFunctions.h"
/*
This .c file contains functions that correspond with specific structs
Everytime you see something like "Item struct:" that indicates that all
functions below it are to only be used for that specified struct
*/

void setTileMap(int *map, int mapHeight, int mapWidth, float tileWidth, float tileHeight, Texture2D tileset) {
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
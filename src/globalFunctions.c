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

void setTileMap(uint8_t *map, int mapHeight, int mapWidth, float tileWidth, float tileHeight, Texture2D tileset){
    for (int y = 0; y < mapHeight; y++) { // every y value it calculates, it then executes all the x values
        for (int x = 0; x < mapWidth; x++) {
            
            // Get the tile number (0-13) for this specific grid coordinate
            int tileIndex = map[(y * mapWidth) + x];

            // Define what part of the PNG to cut out
            // x is 0, y moves down based on the tileIndex (e.g., tile 2 starts at y=16)
            Rectangle sourceRec = { 
                0.0f, // x
                (float)(tileIndex * tileHeight),  // y
                tileWidth, // w
                tileHeight  // h
            };

            // Define where on the virtual canvas to draw it
            Vector2 position = { x * tileWidth, y * tileHeight };

            // Draw the specific 8x8 slice to the virtual canvas
            DrawTextureRec(tileset,   // tile textures
                           sourceRec, // 
                           position, // x, y
                           WHITE // color
                        );
        }
    }
}
#ifndef TEST_CENTER_H
#define TEST_CENTER_H

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include "raylib.h"
#include "../globalFunctions.h"

void initTestCenter(void);
void unloadTestCenter(void);

void testCenterLogic(GameState *currentState, Vector2 mouse);
void testCenterRender();

#endif
#include "raylib.h"
#include "resource_dir.h"	

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <memory>

#include <random>

#include <string> 
#include "Agent.h"
#include "HelpClass.h"

const int screenWidth = 1600;
const int screenHeight = 800;
const int squareSize = (screenWidth + screenHeight) / 20;

using namespace std;
Agent* agent;

void instantiateVariables() {
    agent = new Agent(screenWidth / 2, screenWidth / 6, 25, 5.0f);
}

void moveShape() {
    if (IsKeyDown(KEY_W)) agent->Move(0,-1);
    if (IsKeyDown(KEY_A)) agent->Move(-1, 0);
    if (IsKeyDown(KEY_S)) agent->Move(0, 1);
    if (IsKeyDown(KEY_D)) agent->Move(1, 0);
}

void drawShape() {
    DrawCircle(agent->position.x, agent->position.y, agent->radius, GREEN);
}

int main(void)
{
    InitWindow(screenWidth, screenHeight, "AI Steering Project");
    SetTargetFPS(60);
    instantiateVariables();
    HelperFunctions hf;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText(TextFormat("FPS: %d", GetFPS()), 10, 10, 20, RED);
        moveShape();
        drawShape();
        hf.drawOptions(agent->_currentBehavior);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

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
#include "Player.h"

const int screenWidth = 1600;
const int screenHeight = 800;
const int squareSize = (screenWidth + screenHeight) / 20;

using namespace std;
Agent* agent;
Player* player;

void instantiateVariables() {
    agent = new Agent(Vector2{ screenWidth / 2, screenWidth / 6 }, 25.0f, 5.0f, Vector2{ 0, 0 });
    player = new Player(Vector2{screenWidth / 2, screenWidth / 2}, 25.0f, 8.0f);
}

void drawShape() {
    DrawCircle(player->position.x, player->position.y, player->radius, BLUE);
}

void MovePlayer() {
    if (IsKeyDown(KEY_W)) player->Move(0, -1);
    if (IsKeyDown(KEY_A)) player->Move(-1, 0);
    if (IsKeyDown(KEY_S)) player->Move(0, 1);
    if (IsKeyDown(KEY_D)) player->Move(1, 0);
}

int main(void)
{
    InitWindow(screenWidth, screenHeight, "AI Assignment");
    SetTargetFPS(60);
    instantiateVariables();
    HelperFunctions hf;

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText(TextFormat("FPS: %d", GetFPS()), 10, 10, 20, RED);
        agent->setBehavior();
        agent->updateTarget(player);
        agent->TryToChangeState();
        agent->updateBehavior();
        agent->drawAgent();
        drawShape();
        MovePlayer();

        hf.drawOptions(agent->_currentBehavior);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

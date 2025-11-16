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
#include "Player.h"

#include "ComposedAgent.h"

const int screenWidth = 1800;
const int screenHeight = 1000;
const int squareSize = (screenWidth + screenHeight) / 20;

using namespace std;
Agent* agent;
Player* player;
PathfollowAgent* pfa;

void instantiateVariables() {
    agent = new Agent(Vector2{ screenWidth / 2, screenWidth / 6 }, 25.0f, 5.0f, 0, 0.1f, true);
    player = new Player(Vector2{screenWidth / 2, screenWidth / 2}, 25.0f, 8.0f);
    pfa = new PathfollowAgent(5);
}

// Note agent and player should ALSO deallocate in their deconstructors
void deallocate() {
    delete agent;
    delete player;
}

int main(void)
{
    InitWindow(screenWidth, screenHeight, "AI Assignment");
    SetTargetFPS(60);
    instantiateVariables();

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText(TextFormat("FPS: %d", GetFPS()), 10, 10, 20, RED);
        agent->updateFrame(player);
        player->Update();
        pfa->update();
        EndDrawing();
    }

    deallocate();
    CloseWindow();
    return 0;
}

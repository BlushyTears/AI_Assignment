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

#include "ComposedAgentFollowPath.h"

const int screenWidth = 1800;
const int screenHeight = 1000;
const int squareSize = (screenWidth + screenHeight) / 20;

using namespace std;
Agent* agent;
Player* player;
PathfollowAgent* pfa;
int state = 0;

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

void update() {
    if (state == 0) {
        agent->updateFrame(player);
        player->Update();
    }
    else if (state == 1) {
        pfa->update();
    }
}

void browseStates() {
    string temp = "Assignment part: " + to_string(state) + "/3";
    const char* stringState = temp.c_str();
    DrawText(stringState, screenWidth - 250, screenHeight - 50, 20, RED);

    if (IsKeyPressed(KEY_LEFT) && state != 0) {
        state--;
    }
    else if (IsKeyPressed(KEY_RIGHT) && state != 3) {
        state++;
    }

    if (state == 0) {
        if (IsKeyPressed(KEY_ONE)) agent->_currentBehavior = Seek;
        if (IsKeyPressed(KEY_TWO)) agent->_currentBehavior = Flee;
        if (IsKeyPressed(KEY_THREE)) agent->_currentBehavior = Pursue;

        if (IsKeyPressed(KEY_FOUR)) agent->_currentBehavior = Evade;
        if (IsKeyPressed(KEY_FIVE)) agent->_currentBehavior = Arrive;
        if (IsKeyPressed(KEY_SIX)) agent->_currentBehavior = Wander;
    }
    else if (state == 1) {
        // For future i will change between various states here
    }
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
        browseStates();
        update();
        EndDrawing();
    }

    deallocate();
    CloseWindow();
    return 0;
}

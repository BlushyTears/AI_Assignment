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

#include "ComposedAgents.h"

const int screenWidth = 1800;
const int screenHeight = 1000;
const int squareSize = (screenWidth + screenHeight) / 20;

using namespace std;
Agent* mainAgent;
Player* mainPlayer;

ComposedAgents* ca;
int assignmentPart = 0;

void instantiateVariables() {
    mainAgent = new Agent(Vector2{ screenWidth / 2, screenWidth / 6 }, 25.0f, 5.0f, 0, 0.1f, true);
    mainPlayer = new Player(Vector2{screenWidth / 2, screenWidth / 2}, 25.0f, 8.0f);
    ca = new ComposedAgents();
}

// Note agent and player should ALSO deallocate in their deconstructors
void deallocate() {
    delete mainAgent;
    delete mainPlayer;
    delete ca;
}

void update() {
    if (assignmentPart == 0) {
        mainAgent->updateFrame(mainPlayer);
        mainPlayer->Update();
    }
    else if (assignmentPart == 1) {
        ca->browseStates();
    }
}

// Note: Move this somewhere else at some point
void browseStates() {
    string temp = "Assignment part: " + to_string(assignmentPart + 1) + "/2";
    const char* stringState = temp.c_str();
    DrawText(stringState, screenWidth - 250, screenHeight - 50, 20, RED);

    // Lock state between 0-2 (1-3 for UI)
    if (IsKeyPressed(KEY_LEFT) && assignmentPart != 0) {
        assignmentPart--;
    }
    else if (IsKeyPressed(KEY_RIGHT) && assignmentPart != 1) {
        assignmentPart++;
    }

    if (assignmentPart == 0) {
        mainAgent->displayDebug();
    }
    else if (assignmentPart == 1) {
        ca->displayDebug();
        ca->browseStates();
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

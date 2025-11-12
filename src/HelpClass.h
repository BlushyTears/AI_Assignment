#pragma once

// Nothing here is related to the assignment, 
// it's just a few things to draw cues for various things

#include "Agent.h"
#include "raylib.h"
#include "resource_dir.h"	

#include <iostream>

struct HelperFunctions {
	void drawOptions(Behaviors behavior) {
		switch (behavior) {
		case Seek:
			DrawText("Type: Seek", 10, GetScreenHeight() - 50, 20, RED);
			break;
		case Flee:
			DrawText("Type: Flee", 10, GetScreenHeight() - 50, 20, RED);
			break;
		case Pursue:
			DrawText("Type: Pursue", 10, GetScreenHeight() - 50, 20, RED);
			break;
		case Evade:
			DrawText("Type: Evade", 10, GetScreenHeight() - 50, 20, RED);
			break;
		case Arrive:
			DrawText("Type: Arrive", 10, GetScreenHeight() - 50, 20, RED);
			break;
		case Wander:
			DrawText("Type: Wander", 10, GetScreenHeight() - 50, 20, RED);
			break;
		default:
			DrawText("Error, most likely invalid option picked", 10, GetScreenHeight(), 20, RED);
			break;
		}
	}
};
#pragma once

#include "raylib.h"
#include "raymath.h"
#include "resource_dir.h"

#include "Agent.h"
#include "memory.h"
#include <vector>

struct PathfollowAgent {
	Agent* agent;
	std::vector<Vector2> nodePositions;
	Object* obj;
	int currentNodeIndex = 0;
	int maximumPathCount = 5;
	float width;
	float height;

	PathfollowAgent(int _maximumPathCount) {
		width = GetScreenWidth();
		height = GetScreenHeight();
		agent = new Agent(Vector2{ width / 2, height / 6 }, 25.0f, 5.0f, 0, 0.1f, true);
		obj = new Object(Vector2{ width / 2, height / 4 }, 25.0f, 5.0f);
		maximumPathCount = _maximumPathCount;
		agent->behaviorImpl = std::make_unique<SeekBehavior>();
	}

 	void update() {
		generateNewPath();
		updatePathFollowAgent();
		obj->drawShape();
		debugDraw();
	}

	void generateNewPath() {
		if (nodePositions.empty()) {
			for (int i = 0; i < maximumPathCount; i++) {
				float x = GetRandomValue(0, width);
				float y = GetRandomValue(0, height);
				nodePositions.push_back(Vector2{ x, y });
			}
			currentNodeIndex = 0;
			if (!nodePositions.empty()) obj->position = nodePositions[0];
		}
	}

	// I realized that since i am using unique pointer, that setting seek = std::make_unique<SeekBehavior>()
	// In the constructor would mean I need to use move() and thus now seek is equal nullptr and useless.
	// The ideal solution would be to use shared pointer or copy it as a raw pointer (Tedious) --
	// but that's beyond the scope of this assignment.
	void updatePathFollowAgent() {
		if (nodePositions.size() == 0)
			return;

		if (currentNodeIndex < 0) currentNodeIndex = 0;
		if (currentNodeIndex >= (int)nodePositions.size()) currentNodeIndex = 0;
		Vector2 target = nodePositions[currentNodeIndex];

		if (Vector2Length(agent->position - target) > 10) {
			agent->updateFrame(obj);
		}
		else {
			// Increment, then check if it's time to reset, and only after update obj position
			currentNodeIndex++;
			if (currentNodeIndex >= nodePositions.size()) {
				currentNodeIndex = 0;
				nodePositions.clear();
			}
			obj->position = nodePositions[currentNodeIndex];
		}
	}

	void debugDraw() {
		DrawText(TextFormat("Count: %d", nodePositions.size()), 10, 30, 20, RED);

		if (nodePositions.size() > 0) {
			for (int i = 0; i < nodePositions.size(); i++) {
				DrawCircleLines(nodePositions[i].x, nodePositions[i].y, 10, DARKPURPLE);

				if (i < nodePositions.size() - 1) {
					DrawLine(nodePositions[i].x, nodePositions[i].y,
							 nodePositions[i + 1].x, nodePositions[i + 1].y,
						DARKPURPLE);
				}
			}
		}
	}

	~PathfollowAgent() {
		delete agent;
	}
};
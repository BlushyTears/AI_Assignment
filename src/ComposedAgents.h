#pragma once

#include "raylib.h"
#include "raymath.h"
#include "resource_dir.h"

#include "Agent.h"
#include "memory.h"
#include <vector>

enum AgentBehaviors {
	Pathfollow,
	Separation,
	CollisionAvoidance,
};

struct PathfollowAgent {
	std::vector<Vector2> nodePositions;
	Agent* agent;
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
		if (currentNodeIndex >= nodePositions.size()) currentNodeIndex = 0;
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
				return;
			}
			obj->position = nodePositions[currentNodeIndex];
		}
	}

	void debugDraw() {
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
		delete obj;
	}
};

// I asked a question
struct Separation {
};

// Avoid other moving into agents based on radius basically
struct AgentAvoidance {
	std::vector<Agent*> agentList;
	int numOfAgents;
	const float agentRadius = 25.0f;
	Player* trackedObject;

	// Don't remove this agent->behaviorImpl = std::make_unique<SeekBehavior>();
	AgentAvoidance(int _numOfAgents) {
		trackedObject = new Player(Vector2{ (float)GetScreenWidth() / 2, (float)GetScreenHeight() / 3 }, 25.0f, 8.0f);

		numOfAgents = _numOfAgents;
		for (int i = 0; i < numOfAgents; i++) {
			// Ugly but needed way to spawn agents a bit randomly 
			// (If they spawn inside of each other then they'll stay like that)
			Agent* agent;
			agent = new Agent(Vector2{ (float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2 + (agentRadius * (i + 1)) },
				agentRadius, 5.0f, 0, 0.1f, true);

			// Set agent behavior (Can vary per agent) (Default behavior is seek but we explicitly define it here)
			agent->_currentBehavior = Seek;
			// It's possible to replace the above line with this for instance
			//if (i % 2 == 0)
			//	agent->_currentBehavior = Pursue;
			//else
			//	agent->_currentBehavior = Evade;
			agentList.push_back(agent);
		}
	}

	void update() {
		handleCollision();
		trackedObject->Update();
	}

	void handleCollision() {
		for (int i = 0; i < agentList.size(); i++) {
			for (int j = i + 1; j < agentList.size(); j++) {
				Vector2 diff = agentList[i]->position - agentList[j]->position;
				float distSq = diff.x * diff.x + diff.y * diff.y;

				float minimumDistance = agentList[i]->radius + agentList[j]->radius;
				float minimumDistanceSqared = minimumDistance * minimumDistance;

				if (distSq < minimumDistanceSqared) {
					float dist = sqrtf(distSq);
					Vector2 normal = diff / dist;

					float penetration = (minimumDistance - dist) * 0.5f;

					agentList[i]->position += normal * penetration;
					agentList[j]->position -= normal * penetration;
				}
			}
			agentList[i]->updateFrame(trackedObject);
		}
	}

	~AgentAvoidance() {
		delete trackedObject;
		for (int i = 0; i < agentList.size(); i++) {
			delete agentList[i];
		}
	}
};

// This is the struct that manages the above composed agents
struct ComposedAgents {
	AgentBehaviors currentBehavior;
	PathfollowAgent* pa;
	AgentAvoidance* agentAvoidance;

	ComposedAgents() {
		currentBehavior = Pathfollow;
		pa = new PathfollowAgent(5); // total node paths
		agentAvoidance = new AgentAvoidance(5); // total number of agents displayed
	}

	~ComposedAgents() {
		delete pa;
		delete agentAvoidance;
	}

	void displayDebug() {
		switch (currentBehavior) {
		case Pathfollow:
			pa->update();
			DrawText("1/3 Type: Pathfollow", 10, GetScreenHeight() - 50, 20, RED);
			break;
		case Separation:
			DrawText("2/3 Type: AgentAvoidance", 10, GetScreenHeight() - 50, 20, RED);
			agentAvoidance->update();
			break;
		case CollisionAvoidance:
			DrawText("3/3 Type: Collision Avoidance", 10, GetScreenHeight() - 50, 20, RED);
			break;
		default:
			DrawText("Error, most likely invalid option picked", 10, GetScreenHeight(), 20, RED);
			break;
		}
	}

	void browseStates() {
		if (IsKeyPressed(KEY_ONE)) currentBehavior = Pathfollow;
		if (IsKeyPressed(KEY_TWO)) currentBehavior = Separation;
		if (IsKeyPressed(KEY_THREE)) currentBehavior = CollisionAvoidance;
	}
};

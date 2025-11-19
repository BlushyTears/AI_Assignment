#pragma once

#include "raylib.h"
#include "raymath.h"
#include "resource_dir.h"

#include "Agent.h"
#include "memory.h"
#include <vector>

// Nomenclature: I refer to the term 'composed agent' throughout, 
// Which means I have the fundamental agent behaviors in agent.cpp
// And here I add additional constraints, features such as more agents, obstacles
// Additionally I could have a composed agent that can do both flee and seek with ease
// Which was the main design behind this architecture inspired by the book
enum AgentBehaviors {
	Pathfollow,
	AgentSeparation,
	AgentCollision,
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

// Avoid other moving into agents based on radius basically
// I implemented collision separately since a ghost might not have any collision
// So it made sense to have collision implemented separately
struct SeparatedAgents {
	std::vector<Agent*> agentList;
	int numOfAgents;
	const float agentRadius = 25.0f;
	Player* trackedObject;

	// Don't remove this agent->behaviorImpl = std::make_unique<SeekBehavior>();
	SeparatedAgents(int _numOfAgents) {
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
				float minimumDistanceSqared = minimumDistanceSqared = minimumDistance * minimumDistance;

				if (distSq < minimumDistanceSqared) {
					float dist = sqrtf(distSq);
					Vector2 normal = diff / dist;

					float penetration = (minimumDistance - dist) * 0.5f;
					agentList[i]->position += normal * penetration;
				}
			}
			agentList[i]->updateFrame(trackedObject);
		}
	}

	~SeparatedAgents() {
		delete trackedObject;
		for (int i = 0; i < agentList.size(); i++) {
			delete agentList[i];
		}
	}
};

// Initially I had the idea of abstracting away the walls into this 
// But it ended up cluttering it without really adding anything
struct LineWall {
	Vector2 start;
	Vector2 end;
	Vector2 normal;
	Vector2 normal2;

	LineWall(Vector2 s, Vector2 e) : start(s), end(e) {
		Vector2 dir = Vector2Normalize(end - start);
		normal = Vector2{ -dir.y, dir.x };
		normal2 = normal * -1;
	}
};

// This code basically uses the cone implementation from the book by taking three whiskers or raycasts
struct ObjectAvoidance {
	std::vector<LineWall> walls;
	Agent* agent;
	Player* player;
	Object* dummyObject;

	const int wallCount = 3;
	ObjectAvoidance() {
		agent = new Agent(Vector2{ (float)GetScreenWidth() / 2, (float)GetScreenHeight() / 6 }, 25.0f, 4.0f, 0, 0.1f, true);
		agent->rotationSmoothness = 0.06f;

		player = new Player(Vector2{ (float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2 }, 25.0f, 8.0f);
		dummyObject = new Object(player->position, player->radius, 0);
		walls.push_back(LineWall({ 150, 150 }, {300, 150}));
		walls.push_back(LineWall({ 500, 400 }, {500, 550}));
		walls.push_back(LineWall({ 400, 700 }, {550, 750}));

		agent->behaviorImpl = std::make_unique<SeekBehavior>();
	}

	void update() {
		agent->updateFrame(player);
		player->Update();
		player->drawShape();
		drawWalls();
		avoidWalls();
	}

	void avoidWalls() {
		float rayLength = 100.0f;
		Vector2 ray = agent->position + Vector2Normalize(agent->forwardDirection) * rayLength;
		float fovAngle = 30.0 * DEG2RAD;
		Vector2 forward = Vector2Normalize(agent->forwardDirection);

		// There are plenty of ways of doing this but i decided to go with 
		// one bigger raycast in the center and two smaller ones
		Vector2 whiskers[3] = {
			agent->position + forward * rayLength,
			agent->position + Vector2Rotate(forward, +fovAngle) * rayLength / 4,
			agent->position + Vector2Rotate(forward, -fovAngle) * rayLength / 4,
		};

		bool hit = false;
		Vector2 avoidDir = forward;

		for (int i = 0; i < walls.size(); i++) {
			Vector2 mid = (walls[i].start + walls[i].end) * 0.5f;
			float dist = Vector2Distance(ray, mid);

			for (int i = 0; i < 3; i++) {
				float dist = Vector2Distance(whiskers[i], mid);
				if (dist < agent->radius + 50.0f) {
					avoidDir = Vector2Normalize(agent->position - mid);
					hit = true;
					break;
				}
			}
			if (hit)
				break;
		}

		if (hit) {
			dummyObject->position = agent->position + avoidDir * rayLength;
			agent->playerTarget = dummyObject;
		}
		else
			agent->playerTarget = player;
	}

	void drawWalls() {
		for (int i = 0; i < walls.size(); i++) {
			DrawLine(walls[i].start.x, walls[i].start.y, walls[i].end.x, walls[i].end.y, GREEN);
		}
	}
};

// This is the struct that manages the above composed agents
struct ComposedAgents {
	AgentBehaviors currentBehavior;
	PathfollowAgent* pa;
	SeparatedAgents* separatedAgents;
	ObjectAvoidance* collisionAvoidance;

	ComposedAgents() {
		currentBehavior = Pathfollow;
		pa = new PathfollowAgent(5); 
		separatedAgents = new SeparatedAgents(5);
		collisionAvoidance = new ObjectAvoidance();
	}

	~ComposedAgents() {
		delete pa;
		delete separatedAgents;
		delete collisionAvoidance;
	}

	void displayDebug() {
		switch (currentBehavior) {
		case Pathfollow:
			pa->update();
			DrawText("1/4 Type: Pathfollow", 10, GetScreenHeight() - 50, 20, RED);
			break;
		case AgentSeparation:
			separatedAgents->update();
			DrawText("2/4 Type: Separated Agents Behavior", 10, GetScreenHeight() - 50, 20, RED);
			break;
		case CollisionAvoidance:
			collisionAvoidance->update();
			DrawText("3/4 Type: Collision, agent and Wall Avoidance", 10, GetScreenHeight() - 50, 20, RED);
			break;
		default:
			DrawText("Error, most likely invalid option picked", 10, GetScreenHeight(), 20, RED);
			break;
		}
	}

	void browseStates() {
		if (IsKeyPressed(KEY_ONE)) currentBehavior = Pathfollow;
		if (IsKeyPressed(KEY_TWO)) currentBehavior = AgentSeparation;
		if (IsKeyPressed(KEY_THREE)) currentBehavior = CollisionAvoidance;
	}
};

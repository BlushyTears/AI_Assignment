#pragma once

#include <memory>

#include "raylib.h"
#include "raymath.h"
#include "resource_dir.h"
#include "Player.h"

enum Behaviors {
	Seek,
	Flee,
	Pursue,
	Evade,
	Arrive,
	Wander
};

// Forward declaration but for struct
struct Agent;
struct Player;

// Abstract base class for movement
// Followed by behavior declarations (C++ thing)
struct MovementBehavior {
	virtual ~MovementBehavior() = default;
	virtual void execute(Agent& agent, Player* player) = 0;
};

struct SeekBehavior : MovementBehavior {
	void execute(Agent& agent, Player* player) override;
};

struct FleeBehavior : MovementBehavior{
	void execute(Agent& agent, Player* player) override;
};

struct ArriveBehavior : SeekBehavior {
	void execute(Agent& agent, Player* player) override;
};

struct SteeringOutput {
	Vector2 linearVel;
	float angularVel;
	// Takes in 
	float newOrientation(float currentAgentOrientation, Vector2 targetObject);
};

// Generic agent-related functionality defined here
struct Agent
{
	Vector2 position;
	Vector2 velocity;
	float orientation;
	Vector2 rotation;
	Vector2 forwardDirection;

	int radius;
	float speed;
	Behaviors _currentBehavior;
	std::unique_ptr<MovementBehavior> behaviorImpl;
	Player* playerTarget;
	SteeringOutput steering;

	Agent(Vector2 pos, int initialRadius, float initialSpeed, float initialOrientation);

	void updateFrame(Player* plyr);
	void updateBehavior();
	void setBehavior();
	void drawAgent();
	void TryToChangeState();
}; 


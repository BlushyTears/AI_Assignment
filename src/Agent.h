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
	virtual Vector2 getTargetDirection(Agent& agent, Player* player);
	void execute(Agent& agent, Player* player) override;
};

struct ArriveBehavior : MovementBehavior {
	void execute(Agent& agent, Player* player) override;
	virtual Vector2 getTargetDirection(Agent& agent, Player* player);
};

// Note: From the book page 68 it says this:
// "In our case, when one behavior extends another, 
// it normally does so by calculating an alternative target."
// Hence. the design decision to override of getTargetDirection for
// behaviors that inherit from existing behaviors such as 'flee'.
struct FleeBehavior : SeekBehavior {
	Vector2 getTargetDirection(Agent& agent, Player* player) override;
};

struct PursueBehavior : SeekBehavior {
	Vector2 getTargetDirection(Agent& agent, Player* player);
};

struct EvadeBehavior : PursueBehavior {
	Vector2 getTargetDirection(Agent& agent, Player* player) override;
};

struct WanderBehavior : MovementBehavior {
	float wanderOrientation = 1.0f;
	float wanderOffset = 100;
	float wanderRadius = 25;
	float wanderRate = 0.1f;
	void execute(Agent& agent, Player* player) override;
};

struct SteeringOutput {
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
	float rotationSmoothness;
	float speed;
	Behaviors _currentBehavior;
	Behaviors previousBehavior;
	std::unique_ptr<MovementBehavior> behaviorImpl;
	Player* playerTarget;
	SteeringOutput steering;
	bool drawDebugLines;
	Agent(Vector2 pos, int initialRadius, float initialSpeed, 
	float initialOrientation, float initialRotationSmoothness, bool initiallyDrawDebugLines);

	void OutOfBoundsChecker();
	void updateFrame(Player* plyr);
	void updateBehavior();
	void setBehavior();
	void drawAgent();
	void displayDebug();
	void TryToChangeState();
}; 


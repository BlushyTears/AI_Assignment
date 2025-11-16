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
struct Object;

// Abstract base class for movement
// Followed by behavior declarations (C++ thing)
struct MovementBehavior {
	virtual ~MovementBehavior() = default;
	virtual void execute(Agent& agent, Object* player) = 0;
};

struct SeekBehavior : MovementBehavior {
	virtual Vector2 getTargetDirection(Agent& agent, Object* player);
	void execute(Agent& agent, Object* player) override;
};

struct ArriveBehavior : MovementBehavior {
	void execute(Agent& agent, Object* player) override;
	virtual Vector2 getTargetDirection(Agent& agent, Object* player);
};

// Note: From the book page 68 it says this:
// "In our case, when one behavior extends another, 
// it normally does so by calculating an alternative target."
// Hence. the design decision to override of getTargetDirection for
// behaviors that inherit from existing behaviors such as 'flee'.
struct FleeBehavior : SeekBehavior {
	Vector2 getTargetDirection(Agent& agent, Object* player) override;
};

struct PursueBehavior : SeekBehavior {
	Vector2 getTargetDirection(Agent& agent, Object* player);
};

struct EvadeBehavior : PursueBehavior {
	Vector2 getTargetDirection(Agent& agent, Object* player) override;
};

struct WanderBehavior : MovementBehavior {
	float wanderOrientation = 1.0f;
	float wanderOffset = 250;
	float wanderRadius = 35;
	float wanderRate = 1.0f;
	void execute(Agent& agent, Object* player) override;
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
	Object* playerTarget;
	SteeringOutput steering;
	bool drawDebugLines;
	Agent(Vector2 pos, int initialRadius, float initialSpeed, 
	float initialOrientation, float initialRotationSmoothness, bool initiallyDrawDebugLines);

	void OutOfBoundsChecker();
	void updateFrame(Object* plyr);
	void updateBehavior();
	void setBehavior();
	void drawAgent();
	void displayDebug();
	void TryToChangeState();
}; 


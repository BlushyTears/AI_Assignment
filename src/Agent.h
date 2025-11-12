#pragma once

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

// Generic agent-related functionality defined here
struct Agent
{
	Vector2 position;
	int radius;
	float speed;
	Behaviors _currentBehavior;
	std::unique_ptr<MovementBehavior> behaviorImpl;
	Player* playerTarget;

	Agent(Vector2 pos, int _radius, float _speed) {
		position = pos;
		radius = _radius;
		speed = _speed;
		_currentBehavior = Seek;
		playerTarget = nullptr;
		behaviorImpl = nullptr;
	}

	void updateTarget(Player* plyr) {
		playerTarget = plyr;
	}

	void updateBehavior() {
		if (behaviorImpl && playerTarget)
			behaviorImpl->execute(*this, playerTarget);
	}

	void setBehavior() {
		switch (_currentBehavior) {
		case Seek:
			DrawText("Type: Seek", 10, GetScreenHeight() - 50, 20, RED);
			behaviorImpl = std::make_unique<SeekBehavior>();
			break;
		case Flee:
			DrawText("Type: Flee", 10, GetScreenHeight() - 50, 20, RED);
			behaviorImpl = std::make_unique<FleeBehavior>();
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

	void drawAgent() {
		DrawCircle(position.x, position.y, radius, GREEN);
	}

	void TryToChangeState() {
		if (IsKeyPressed(KEY_ONE)) _currentBehavior = Seek;
		if (IsKeyPressed(KEY_TWO)) _currentBehavior = Flee;
		if (IsKeyPressed(KEY_THREE)) _currentBehavior = Pursue;

		if (IsKeyPressed(KEY_FOUR)) _currentBehavior = Evade;
		if (IsKeyPressed(KEY_FIVE)) _currentBehavior = Arrive;
		if (IsKeyPressed(KEY_SIX)) _currentBehavior = Wander;
	}
}; 

/*
	Movement behavior implementations below:
*/ 

inline void SeekBehavior::execute(Agent& agent, Player* player) {
	Vector2 dir = Vector2Normalize(Vector2Subtract(player->position, agent.position));
	agent.position = Vector2Add(agent.position, Vector2Scale(dir, agent.speed));
}

inline void FleeBehavior::execute(Agent& agent, Player* player) {
	Vector2 dir = Vector2Normalize(Vector2Subtract(agent.position, player->position));
	agent.position = Vector2Add(agent.position, Vector2Scale(dir, agent.speed));
}

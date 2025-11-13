#include "raylib.h"
#include "raymath.h"
#include "resource_dir.h"

#include "Agent.h"

// First couple of AI movement behaviors defined below
void SeekBehavior::execute(Agent& agent, Player* player) {

	float smoothRotation = 0.06f;
	Vector2 toTarget = player->position - agent.position;
	float desiredRotation = agent.steering.newOrientation(agent.orientation, toTarget);

	float delta = desiredRotation - agent.orientation;
	if (delta > PI) delta -= 2 * PI;
	else if (delta < -PI) delta += 2 * PI;

	agent.orientation += delta * smoothRotation;
	agent.forwardDirection = { cosf(agent.orientation), sinf(agent.orientation) };

	Vector2 desiredVelocity = agent.forwardDirection * agent.speed;
	Vector2 steering = desiredVelocity - agent.velocity;
	steering = Vector2ClampValue(steering, 0, 0.1f);

	agent.velocity += steering;
	agent.position += agent.velocity;
}

void FleeBehavior::execute(Agent& agent, Player* player) {
	float smoothRotation = 0.06f;
	Vector2 toTarget = agent.position - player->position;
	float desiredRotation = agent.steering.newOrientation(agent.orientation, toTarget);

	float delta = desiredRotation - agent.orientation;
	if (delta > PI) delta -= 2 * PI;
	else if (delta < -PI) delta += 2 * PI;

	agent.orientation += delta * smoothRotation;
	agent.forwardDirection = { cosf(agent.orientation), sinf(agent.orientation) };

	Vector2 desiredVelocity = agent.forwardDirection * agent.speed;
	Vector2 steering = desiredVelocity - agent.velocity;
	steering = Vector2ClampValue(steering, 0, 0.1f);

	agent.velocity += steering;
	agent.position += agent.velocity;
}

void ArriveBehavior::execute(Agent& agent, Player* player) {
	float distance = Vector2Length(player->position - agent.position);

	DrawText(TextFormat("FPS: %.1f ", distance), 10, 30, 20, GREEN);

	if (distance > 200.0f) {
		SeekBehavior::execute(agent, player);
	}
	else {
		// stand still
	}
}

// Generic Agent related:
float SteeringOutput::newOrientation(float currentAgentOrientation, Vector2 targetObject) {
	const float eps = 0.001f;
	if (Vector2Length(targetObject) > eps) {
		return atan2(targetObject.y, targetObject.x);
	}
	else {
		return currentAgentOrientation;
	}
}

Agent::Agent(Vector2 pos, int initialRadius, float initialSpeed, float initialOrientation) {
	position = pos;
	radius = initialRadius;
	speed = initialSpeed;
	orientation = initialOrientation;
	velocity = { 0, 0 };
	rotation = { 0, 0 };
	_currentBehavior = Seek;
	playerTarget = nullptr;
	behaviorImpl = nullptr;
}

void Agent::updateFrame(Player* plyr) {
	updateBehavior();
	drawAgent();
	TryToChangeState();
	setBehavior();
	playerTarget = plyr;
}

void Agent::updateBehavior() {
	if (behaviorImpl && playerTarget)
		behaviorImpl->execute(*this, playerTarget);
}

void Agent::setBehavior() {
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
		behaviorImpl = std::make_unique<ArriveBehavior>();
		break;
	case Wander:
		DrawText("Type: Wander", 10, GetScreenHeight() - 50, 20, RED);
		break;
	default:
		DrawText("Error, most likely invalid option picked", 10, GetScreenHeight(), 20, RED);
		break;
	}
}
void Agent::drawAgent() {
	DrawCircle(position.x, position.y, radius, GREEN);
	DrawLineV(position, Vector2Add(position, Vector2Scale(forwardDirection, 50.0f)), RED);
}

void Agent::TryToChangeState() {
	if (IsKeyPressed(KEY_ONE)) _currentBehavior = Seek;
	if (IsKeyPressed(KEY_TWO)) _currentBehavior = Flee;
	if (IsKeyPressed(KEY_THREE)) _currentBehavior = Pursue;

	if (IsKeyPressed(KEY_FOUR)) _currentBehavior = Evade;
	if (IsKeyPressed(KEY_FIVE)) _currentBehavior = Arrive;
	if (IsKeyPressed(KEY_SIX)) _currentBehavior = Wander;
}
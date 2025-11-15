#include "raylib.h"
#include "raymath.h"
#include "resource_dir.h"

#include "Agent.h"

// This boilerplate purely exists so we can override it
Vector2 SeekBehavior::getTargetDirection(Agent& agent, Player* player) {
	return player->position - agent.position;
}

// First couple of AI movement behaviors defined below
void SeekBehavior::execute(Agent& agent, Player* player) {
	agent.rotationSmoothness = 0.2f;
	Vector2 toTarget = getTargetDirection(agent, player);
	float desiredRotation = agent.steering.newOrientation(agent.orientation, toTarget);

	float delta = desiredRotation - agent.orientation;
	if (delta > PI) delta -= 2 * PI;
	else if (delta < -PI) delta += 2 * PI;

	agent.orientation += delta * agent.rotationSmoothness;
	agent.forwardDirection = { cosf(agent.orientation), sinf(agent.orientation) };

	Vector2 desiredVelocity = agent.forwardDirection * agent.speed;
	Vector2 steering = desiredVelocity - agent.velocity;
	steering = Vector2ClampValue(steering, 0, 0.2f);

	agent.velocity += steering;
	agent.position += agent.velocity;
}

// Note: FleeBehavior impl just overwrites the targetDirection
Vector2 FleeBehavior::getTargetDirection(Agent& agent, Player* player) {
	return agent.position - player->position;
}

// Pursue inherits from seek but ovewrrites targetDirection with a prediction
Vector2 PursueBehavior::getTargetDirection(Agent& agent, Player* player) {
	// Hard Setting rotation smoothness isn't ideal,
	// but I had to make it react faster when making predictions
	agent.rotationSmoothness = 0.2f;
	float maxPrediction = 50.0f;
	Vector2 toTarget = player->position - agent.position;
	float distance = Vector2Length(toTarget);

	float prediction;
	if (distance <= maxPrediction)
		return toTarget;
	else if (agent.speed <= distance / maxPrediction)
		prediction = maxPrediction;
	else
		prediction = distance / agent.speed;

	toTarget += player->GetVelocity() * prediction;
	return toTarget;
}

// Bit ugly copy paste below, but I didn't realize pursue would need to inherit from seek and then evade inherit from puruse
// As I only planned this whole architechture to be one layer of inheritence at most
Vector2 EvadeBehavior::getTargetDirection(Agent& agent, Player* player) {
	agent.rotationSmoothness = 0.2f;
	float maxPrediction = 50.0f;
	Vector2 toTarget = agent.position - player->position;
	float distance = Vector2Length(toTarget);

	float prediction;
	if (distance <= maxPrediction)
		return toTarget;
	else if (agent.speed <= distance / maxPrediction)
		prediction = maxPrediction;
	else
		prediction = distance / agent.speed;

	toTarget += player->GetVelocity() * prediction;
	return toTarget;
}

// Note: ArriveBehavior boilerplate
Vector2 ArriveBehavior::getTargetDirection(Agent& agent, Player* player) {
	return player->position - agent.position;
}

void ArriveBehavior::execute(Agent& agent, Player* player) {
	Vector2 direction = player->position - agent.position;
	float distance = Vector2Length(direction);

	agent.rotationSmoothness = 0.12f;
	Vector2 toTarget = getTargetDirection(agent, player);
	float desiredRotation = agent.steering.newOrientation(agent.orientation, toTarget);

	float delta = desiredRotation - agent.orientation;
	if (delta > PI) delta -= 2 * PI;
	else if (delta < -PI) delta += 2 * PI;

	agent.orientation += delta * agent.rotationSmoothness;
	agent.forwardDirection = { cosf(agent.orientation), sinf(agent.orientation) };

	float slowdownSpeed = agent.speed;
	if (distance < 200)
		// The author didn't explain things properly with slowdown, 
		// so i added a subtraction bias of -2 because agent was moving a tiny bit otherwise
		slowdownSpeed = ((distance / 200) * agent.speed) - 2.0f;

	// Hacky solution to make agent really stop and not crawl (It's a cheat to make the AI appear smart!)
	if (slowdownSpeed <= 0.2f)
		slowdownSpeed = 0;

	Vector2 desiredVelocity = agent.forwardDirection * slowdownSpeed;
	Vector2 steering = desiredVelocity - agent.velocity;
	steering = Vector2ClampValue(steering, 0, 0.1f);

	agent.velocity += steering;
	agent.position += agent.velocity;
}

// Wander unfortunately can't inherit from seek it seems
void WanderBehavior::execute(Agent& agent, Player* player) {
	agent.rotationSmoothness = 0.2f;
	int binomial = GetRandomValue(-1, 1);

	wanderOrientation += float(binomial / wanderRate);
	float targetOrientation = wanderOrientation + agent.orientation;

	Vector2 target = agent.position + Vector2{ cosf(agent.orientation), sinf(agent.orientation) } * wanderOffset;
	target += Vector2{ cosf(targetOrientation), sinf(targetOrientation) } * wanderRadius;

	Vector2 direction = target - agent.position;
	float desiredRotation = agent.steering.newOrientation(agent.orientation, target - agent.position);

	if (agent.drawDebugLines) {
		DrawCircle(target.x, target.y, wanderRadius, BLUE);
		DrawLine(agent.position.x, agent.position.y, target.x, target.y, BLUE);
	}
	
	float delta = desiredRotation - agent.orientation;
	if (delta > PI) delta -= 2 * PI;
	else if (delta < -PI) delta += 2 * PI;

	agent.orientation += delta * agent.rotationSmoothness;
	agent.forwardDirection = { cosf(agent.orientation), sinf(agent.orientation) };

	Vector2 desiredVelocity = agent.forwardDirection * agent.speed;
	Vector2 steering = desiredVelocity - agent.velocity;
	steering = Vector2ClampValue(steering, 0, 0.5f);

	agent.velocity += steering;
	agent.position += agent.velocity;
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

Agent::Agent(Vector2 pos, int initialRadius, float initialSpeed, float initialOrientation,
					float initialRotationSmoothness, bool initiallyDrawDebugLines) {
	position = pos;
	radius = initialRadius;
	speed = initialSpeed;
	orientation = initialOrientation;
	rotationSmoothness = initialRotationSmoothness;
	drawDebugLines = initiallyDrawDebugLines;
	velocity = { 0, 0 };
	rotation = { 0, 0 };
	_currentBehavior = Seek;
	previousBehavior = Flee;
	playerTarget = nullptr;
	behaviorImpl = nullptr;
}

void Agent::OutOfBoundsChecker() {
	float width = GetScreenWidth();
	float height = GetScreenHeight();

	if (position.x > width) position.x = 0;
	else if (position.x < 0) position.x = width;

	if (position.y > height) position.y = 0;
	else if (position.y < 0) position.y = height;
}

void Agent::updateFrame(Player* plyr) {
	TryToChangeState();
	updateBehavior();
	setBehavior();
	drawAgent();
	displayDebug();
	OutOfBoundsChecker();
	playerTarget = plyr;
}

void Agent::updateBehavior() {
	if (behaviorImpl && playerTarget)
		behaviorImpl->execute(*this, playerTarget);
}

// I've hard coded the behaviors down here, 
// But the idea with this architechture would be to
// Pass in the behaviors into the function like this:
// 
// setBehavior(MovementBehavior mb)
// And then via polymorphism each agent would be performing
// A behavior without really knowing which kind of behavior
// Since it's just accepting a pointer
// 
// The author does mention polymorphism as a design approach 
// So this was my interpertation to it.
void Agent::setBehavior() {
	// Only update agent behavior if enum changed
	if (_currentBehavior == previousBehavior)
		return;
	previousBehavior = _currentBehavior;
	switch (_currentBehavior) {
	case Seek:
		behaviorImpl = std::make_unique<SeekBehavior>();
		break;
	case Flee:
		behaviorImpl = std::make_unique<FleeBehavior>();
		break;
	case Pursue:
		behaviorImpl = std::make_unique<PursueBehavior>();
		break;
	case Evade:
		behaviorImpl = std::make_unique<EvadeBehavior>();
		break;
	case Arrive:
		behaviorImpl = std::make_unique<ArriveBehavior>();
		break;
	case Wander:
		behaviorImpl = std::make_unique<WanderBehavior>();
		break;
	default:
		DrawText("Error, most likely invalid option picked", 10, GetScreenHeight(), 20, RED);
		break;
	}
}

void Agent::TryToChangeState() {
	if (IsKeyPressed(KEY_ONE)) _currentBehavior = Seek;
	if (IsKeyPressed(KEY_TWO)) _currentBehavior = Flee;
	if (IsKeyPressed(KEY_THREE)) _currentBehavior = Pursue;

	if (IsKeyPressed(KEY_FOUR)) _currentBehavior = Evade;
	if (IsKeyPressed(KEY_FIVE)) _currentBehavior = Arrive;
	if (IsKeyPressed(KEY_SIX)) _currentBehavior = Wander;
}

void Agent::drawAgent() {
	DrawCircle(position.x, position.y, radius, GREEN);
	DrawLineV(position, Vector2Add(position, Vector2Scale(forwardDirection, 50.0f)), RED);
}

void Agent::displayDebug() {
	// Toggle debug
	if (IsKeyPressed(KEY_P)) {
		drawDebugLines = !drawDebugLines;
	}
	if (drawDebugLines) {
		switch (_currentBehavior) {
		case Seek:
			DrawText("1/6 Type: Seek", 10, GetScreenHeight() - 50, 20, RED);
			break;
		case Flee:
			DrawText("2/6 Type: Flee", 10, GetScreenHeight() - 50, 20, RED);
			break;
		case Pursue:
			DrawText("3/6 Type: Pursue", 10, GetScreenHeight() - 50, 20, RED);
			break;
		case Evade:
			DrawText("4/6 Type: Evade", 10, GetScreenHeight() - 50, 20, RED);
			break;
		case Arrive:
			DrawText("5/6 Type: Arrive", 10, GetScreenHeight() - 50, 20, RED);
			break;
		case Wander:
			DrawText("6/6 Type: Wander", 10, GetScreenHeight() - 50, 20, RED);
			break;
		default:
			DrawText("Error, most likely invalid option picked", 10, GetScreenHeight(), 20, RED);
			break;
		}
	}

}
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
	CollisionAvoidance,
	AgentJumping,
};

struct PathfollowAgent {
	std::vector<Vector2> nodePositions;
	Agent* agent;
	Object* obj;
	int currentNodeIndex = 0;
	int maximumPathCount = 5;
	float width;
	float height;

	PathfollowAgent(int _maximumPathCount);
	~PathfollowAgent();
	void update();
	void generateNewPath();
	void updatePathFollowAgent();
	void debugDraw();
};

// Avoid other moving into agents based on radius basically
// I implemented collision separately since a ghost might not have any collision
// So it made sense to have collision implemented separately
struct SeparatedAgents {
	std::vector<Agent*> agentList;
	int numOfAgents;
	const float agentRadius = 25.0f;
	Player* trackedObject;

	SeparatedAgents();
	SeparatedAgents(int _numOfAgents);

	virtual void update();
	virtual float getMinDistance(int dist1, int dist2);
	void handleCollision();
	virtual ~SeparatedAgents();
};

// Initially I had the idea of abstracting away the walls into this 
// But it ended up cluttering it without really adding anything
struct LineWall {
	Vector2 start;
	Vector2 end;

	LineWall(Vector2 s, Vector2 e) : start(s), end(e) {};
};

// This code basically uses the cone implementation from the book by taking three whiskers or raycasts
// If we hit a wall we just change the target position
struct ObjectAvoidance : SeparatedAgents {
	std::vector<LineWall> walls;
	Object* dummyObject;

	const int wallCount = 3;

	ObjectAvoidance(int numAgents);
	~ObjectAvoidance();
	void update() override;
	float getMinDistance(int dist1, int dist2) override;
	void avoidWalls();
	void drawWalls();
};

struct Pad {
	Vector2 position;
	Vector2 size;

	Pad(Vector2 _position, Vector2 _size);
	bool isAgentOnThisPad(Agent* agent, float padRadius);
	void drawPad(Color _color);
};

// Basic agent with defualt behavior (Seek) which will jump over some edge to get to player
// I didn't quite understand the nuance on what we had to do, but i just did something simple with a jumping pad
// And check if the agent is on the jumping pad, then it will jump. Naturally this would probably be hidden.
// Potentially it would make more sense to temporarily change the direction of the agent towards the jumping pad
// But I wasn't sure if I should do that (It's quite easy to do and would just make it harder to test if the agent doesn't jump)
struct JumpingAgent {
	Agent* agent;
	Player* player;
	Pad* pad;
	Pad* deathPad;
	Vector2 jumpAcceleration;
	float jumpSpeed;
	float zPosition;
	bool hasJumped;
	float gravity;
	float baseRadius;

	JumpingAgent();
	~JumpingAgent();

	void jump();
	void respawn();
	void applyJumpPhysics();
	void checkPads();
	void drawPads();
	void update();
};

// This is the struct that manages the above composed agents
struct ComposedAgents {
	AgentBehaviors currentBehavior;

	PathfollowAgent* pathFollowBehavior;
	SeparatedAgents* separatedAgentsBehavior;
	ObjectAvoidance* collisionAvoidanceBehavior;
	JumpingAgent* jumpingBehavior;

	ComposedAgents();
	~ComposedAgents();

	void displayDebug();
	void browseStates();
};
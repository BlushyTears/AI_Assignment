#pragma once

enum Behaviors {
	Seek,
	Flee,
	Pursue,
	Evade,
	Arrive,
	Wander
};

struct Agent
{
	Vector2 position;
	int radius;
	float speed;
	Behaviors _currentBehavior;

	Agent(Vector2 pos, int _radius, float _speed) {
		position = pos;
		radius = _radius;
		speed = _speed;
		_currentBehavior = Wander;
	}

	void Move(int x, int y) {
		position.x += x * speed;
		position.y += y * speed;
	}
};


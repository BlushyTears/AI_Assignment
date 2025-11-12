#pragma once

struct Agent
{
	int pos_x;
	int pos_y;
	int _radius;
	float _speed;

	Agent(int x, int y, int radius, float speed) {
		pos_x = x;
		pos_y = y;
		_radius = radius;
		_speed = speed;
	}

	void Move(int x, int y) {
		pos_x += x * _speed;
		pos_y += y * _speed;
	}
};


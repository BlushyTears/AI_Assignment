#pragma once

#include "raylib.h"
#include "raymath.h"
#include "resource_dir.h"	

struct Player {
    Vector2 position;
    float speed;
    float radius;
    Player(Vector2 startingPos, float startingRadius, float startingSpeed) {
        position = startingPos;
        radius = startingRadius;
        speed = startingSpeed;
    }

    void Move(int x, int y) {
        position.x += x * speed;
        position.y += y * speed;
    }
};

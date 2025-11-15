#pragma once

#include "raylib.h"
#include "raymath.h"
#include "resource_dir.h"	

struct Player {
    Vector2 position;
    Vector2 velocity;
    float speed;
    float radius;

    Player(Vector2 startingPos, float startingRadius, float startingSpeed) {
        position = startingPos;
        velocity = { 0, 0 };
        radius = startingRadius;
        speed = startingSpeed;
    }

    void Update() {
        TakeInput();
        drawShape();
    }

    void TakeInput() {
        int x = 0;
        int y = 0;

        if (IsKeyDown(KEY_W)) y -= 1;
        if (IsKeyDown(KEY_S)) y += 1;
        if (IsKeyDown(KEY_A)) x -= 1;
        if (IsKeyDown(KEY_D)) x += 1;

        Move(x, y);
    }

    void Move(int x, int y) {
        velocity.x = x * speed;
        velocity.y = y * speed;

        position.x += velocity.x;
        position.y += velocity.y;
    }

    void drawShape() {
        DrawCircle(position.x, position.y, radius, BLUE);
    }

    Vector2 GetVelocity() const {
        return velocity;
    }
};

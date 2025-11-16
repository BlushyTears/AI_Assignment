#pragma once

#include "raylib.h"
#include "raymath.h"
#include "resource_dir.h"	


// The reason player inherits from object, is so that the agent can get its position
// Naturally it might make more sense to just parse the position for the agent,
// But the book made me utilize thigns like velocity, so out of caution I just parsed everything more or less.
// This naturally led to the annoyance of objects that Actually have to be static (Like a list of path points)
// However they weren't static since they were initially taking keyboard input since I was parsing player
// Which is why I split up Object and Player but still have Agent treat both the same

struct Object {
    Vector2 position;
    Vector2 velocity;
    float radius;
    float speed;

    Object(Vector2 startingPos, float startingRadius, float startingSpeed) {
        position = startingPos;
        radius = startingRadius;
        speed = startingSpeed;
        velocity = { 0, 0 };
    }

    virtual void Update() {
        drawShape();
    }

    virtual void drawShape() {
        DrawCircle(position.x, position.y, radius, DARKGRAY);
    }

    Vector2 GetVelocity() const {
        return velocity;
    }
};

struct Player : Object {
    void TakeInput() {
        int x = 0;
        int y = 0;

        if (IsKeyDown(KEY_W)) y -= 1;
        if (IsKeyDown(KEY_S)) y += 1;
        if (IsKeyDown(KEY_A)) x -= 1;
        if (IsKeyDown(KEY_D)) x += 1;

        Move(x, y);
    }

    void Update() override {
        drawShape();
        TakeInput();
    }

    void Move(int x, int y) {
        velocity.x = x * speed;
        velocity.y = y * speed;

        position.x += velocity.x;
        position.y += velocity.y;
    }
    
    void drawShape() override {
        DrawCircle(position.x, position.y, radius, BLUE);
    }

    Player(Vector2 startingPos, float startingRadius, float startingSpeed)
        : Object(startingPos, startingRadius, startingSpeed) {
    }
};


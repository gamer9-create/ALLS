#include <iostream>
#include "raylib.h"
#include "raymath.h"
#include "network/Client.h"
#include "network/Server.h"

void client() {
    InitWindow(640, 480, "SO GRAB A PLATE, HAVE A TASTE");
    SetTargetFPS(60);

    StartClient();
    Rectangle MyPlayerHitbox = {0, 0, 40, 40};
    float MyPlayerSpeed = 150;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);
        DrawText(std::to_string(GetTime()).c_str(), 0, 0, 20, BLACK);

        Vector2 Movement = {0, 0};
        if (IsKeyDown(KEY_A))
            Movement.x -= 1;
        if (IsKeyDown(KEY_D))
            Movement.x += 1;
        if (IsKeyDown(KEY_W))
            Movement.y -= 1;
        if (IsKeyDown(KEY_S))
            Movement.y += 1;
        Movement = Vector2Normalize(Movement);
        MyPlayerHitbox.x += Movement.x * MyPlayerSpeed * GetFrameTime();
        MyPlayerHitbox.y += Movement.y * MyPlayerSpeed * GetFrameTime();

        UpdatePosition(MyPlayerHitbox.x, MyPlayerHitbox.y);

        std::vector<Vector2> positions = GetPositions();
        positions.push_back({MyPlayerHitbox.x, MyPlayerHitbox.y});
        for (Vector2 position : positions) {
            DrawRectangle(position.x, position.y, 40, 40, RED);
        }

        EndDrawing();
    }

    StopClient();

    CloseWindow();
}

void server() {
    StartServer();
    StopServer();
}

int main(int argc, char** argv) {
    std::string arg = argv[1];
    if (arg== "server") {
        printf("Server selected.\n");
        server();
    } else {
        printf("Client selected.\n");
        client();

    }
    return 0;
}
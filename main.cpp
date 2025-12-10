#include <float.h>
#include <iostream>
#include "raylib.h"
#include "raymath.h"
#include "network/Client.h"
#include "network/Server.h"
#include "network/Utils.h"

using namespace std;

void process_player(long id, Player* player) {
    player->LocalState.id = player->CurrentState.id;

    double render_time = GetServerTime() - 3.0f;
    //cout << render_time << setprecision(1000) << endl;
    PlayerState *lowest = new PlayerState();
    lowest->timestamp = -1;
    PlayerState *highest = new PlayerState();
    highest->timestamp = FLT_MAX;

    bool found = false;
    bool found2 = false;

    for (PlayerState &s : player->PreviousPlayerStates) {
        if (s.timestamp < render_time && s.timestamp > lowest->timestamp) {
            //cout << "time stamp " << s.timestamp << ", render time: " << render_time << setprecision(100000) << endl;
            lowest = &s;
            found = true;
        }
    }

    for (PlayerState &s : player->PreviousPlayerStates) {
        if (s.timestamp > render_time && s.timestamp < highest->timestamp && s.position != lowest->position) {
            highest = &s;
            found2 = true;
        }
    }

    cout << found << " " << found2 << endl;

    if (found && found2) {
        double time_diff = highest->timestamp - lowest->timestamp;
        float prog = (render_time - lowest->timestamp) / time_diff;

        player->LocalState.position = {lowest->position.x + (highest->position.x - lowest->position.x) * prog, lowest->position.y + (highest->position.y - lowest->position.y) * prog};
        cout << lowest->position.x << ", " << lowest->position.y << ", " << highest->position.x << ", " << highest->position.y << ", progress: " << prog << endl;
    }

    DrawRectangle(player->LocalState.position.x, player->LocalState.position.y, 40, 40, RED);
}

void client() {
    InitWindow(640, 480, "SO GRAB A PLATE, HAVE A TASTE");
    SetTargetFPS(60);

    StartClient();
    Player MyPlayer = Player();
    Rectangle MyPlayerHitbox = {0, 0, 40, 40};
    float MyPlayerSpeed = 150;
    float MyPlayerRotation = 0;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);

        DrawText(std::to_string(GetServerTime()).c_str(), 0, 0, 20, BLACK);

        Vector2 MyPlayerVelocity = {0, 0};
        if (IsKeyDown(KEY_A))
            MyPlayerVelocity.x -= 1;
        if (IsKeyDown(KEY_D))
            MyPlayerVelocity.x += 1;
        if (IsKeyDown(KEY_W))
            MyPlayerVelocity.y -= 1;
        if (IsKeyDown(KEY_S))
            MyPlayerVelocity.y += 1;
        MyPlayerVelocity = Vector2Normalize(MyPlayerVelocity);
        MyPlayerVelocity = Vector2Multiply(MyPlayerVelocity, {MyPlayerSpeed, MyPlayerSpeed});
        MyPlayerHitbox.x += MyPlayerVelocity.x * GetFrameTime();
        MyPlayerHitbox.y += MyPlayerVelocity.y * GetFrameTime();

        MyPlayer.CurrentState.position = {MyPlayerHitbox.x, MyPlayerHitbox.y};
        MyPlayer.CurrentState.velocity = MyPlayerVelocity;
        MyPlayer.CurrentState.rotation = MyPlayerRotation;
        MyPlayer.CurrentState.speed = MyPlayerSpeed;
        MyPlayer.LocalState = MyPlayer.CurrentState;
        UpdateState(MyPlayer.CurrentState);

        for (auto &[id, player] : (*GetPlayers())) {
            process_player(id,&player);
        }
        DrawRectangle(MyPlayer.LocalState.position.x, MyPlayer.LocalState.position.y, 40, 40, RED);

        MyPlayer.LastState = MyPlayer.CurrentState;

        EndDrawing();
    }

    std::cout << "stopping client!" << std::endl;
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
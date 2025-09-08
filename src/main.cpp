#include "raylib.h"

void t1 () {
    InitWindow(640, 400, "haustier-emu");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_Q)) {
            break;
        }

        BeginDrawing();

        ClearBackground(WHITE);

        EndDrawing();
    }

    CloseWindow();
}

int main () {
    t1();
    return 0;
}
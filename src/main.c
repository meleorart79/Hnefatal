
#include <stdlib.h>
#include <time.h>
#include "raylib.h"
#include "game.h"
#include "render.h"

int main(void)
{
    srand((unsigned)time(NULL));
    const char* title = TextFormat("%s v%s", game_title(), game_version());
    InitWindow(SCREEN_W, SCREEN_H, title);
    SetTargetFPS(60);

    GameState game;
    game_init(&game);
    RenderState view = { 0 };

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        if (IsKeyPressed(KEY_SPACE)) game_init(&game);   /* redistribuer (debug) */
        render_update(&view, dt);

        BeginDrawing();
        render_draw(&view, &game);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}


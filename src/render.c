
#include <stdbool.h>
#include "raylib.h"
#include "render.h"

static void draw_board(const Board* board)
{

}

void render_update(RenderState* view, float dt)
{
    Vector2 mouse = GetMousePosition();
    view->hovered = -1;

    for (int i = 0; i < HAND_SIZE; i++) {
        bool over = CheckCollisionPointRec(mouse, hand_piece_rect(i));
        if (over) view->hovered = i;

        float target = over ? 24.0f : 0.0f;
        float k = 12.0f * dt;
        if (k > 1.0f) k = 1.0f;
        view->lift[i] += (target - view->lift[i]) * k;
    }
}

void render_draw(const RenderState* view, const GameState* game)
{
    ClearBackground((Color) { 24, 28, 34, 255 });

    draw_board

    DrawText(TextFormat("boardp: %d", game->boardp.count), 10, 10, 20, RAYWHITE);
    DrawFPS(10, 36);
}


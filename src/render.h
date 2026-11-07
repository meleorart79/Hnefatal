
#ifndef RENDER_H
#define RENDER_H

#include "game.h"

#define SCREEN_W 1280
#define SCREEN_H 720

typedef struct {
    float lift[HAND_SIZE];   /* hauteur de survol de chaque carte (pixels) */
    int   hovered;           /* index de la carte survolee, -1 si aucune */
} RenderState;

void render_update(RenderState* view, float dt);
void render_draw(const RenderState* view, const GameState* game);

#endif


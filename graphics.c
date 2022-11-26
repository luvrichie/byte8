//
// Created by richie on 23/11/2022.
//
#include <raylib.h>
#include "include/chip8.h"
#include "include/graphics.h"
#include <stdio.h>

void clear_display_ray() {
    ClearBackground(LIGHTGRAY);
};

void draw_sprite_ray(chip_8 *c) {
    BeginDrawing();
    ClearBackground(LIGHTGRAY);
    for (int x = 0; x < 64; x++) {
        for (int y = 0; y < 32; y++) {
            if (c->display[x][y]) {
                DrawRectangle(x*10, y*10, 10, 10, BLACK);
            }
        }
    }
    EndDrawing();
}

void init_gfx() {
    InitWindow(WIN_WIDTH, WIN_HEIGHT, TITLE);
}
//
// Created by richie on 23/11/2022.
//

#ifndef BYTE8_GRAPHICS_H
#define BYTE8_GRAPHICS_H

#include "chip8.h"
#include <raylib.h>

#define WIN_SCALE 10
#define WIN_WIDTH DISPLAY_WIDTH*WIN_SCALE
#define WIN_HEIGHT DISPLAY_HEIGHT*WIN_SCALE

#define TITLE "byte8 | powered by raylib"

void init_gfx();
void clear_display_ray();
void draw_sprite_ray(chip_8 *c);

#endif //BYTE8_GRAPHICS_H

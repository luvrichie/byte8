//
// Created by richie on 23/11/2022.
//
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "include/chip8.h"
#include "include/opcodes.h"
#include <stdbool.h>
#include <raylib.h>
#include <unistd.h>

// helper func
void push(chip_8 *c, uint16_t value) {
    if (c->sp + 1 >= STACK_SIZE - 1) {
        fprintf(stderr, "stack overflow!!!");
        return;
    }

    c->sp++;
    c->stack[c->sp] = value;
}

uint16_t pop(chip_8 *c) {
    if (c->sp == 0) {
        fprintf(stderr, "stack underflow!!!");
        return -1;
    }
    uint16_t result = c->stack[c->sp];
    c->sp--;
    return result;
}

int current_key_pressed(const uint8_t keys[]) {
    int result = -1;

    for (int i = 0; i < 16; i++) {
        if (keys[i] > 0)
            return i;
    }
    return result;
}

// 00E0: clear display
// the title says it all; clears chip-8's display.
void clear_display(chip_8 *c) {
    memset(c->display, 0, 64 * 32);
}

// 00EE: return from subroutine
// pop the last address from stack and set the PC to it
void return_subroutine(chip_8 *c) {
    c->pc = pop(c);
}

// 1NNN: jump to address
// to say it differently: sets the program counter to the address provided.
void jmp_addr(chip_8 *c, uint16_t addr) {
    c->pc = addr;
}

// 2NNN: calls subroutine
// In other words, just like 1NNN, you should set PC to NNN. However, the difference between a jump and a call is that this instruction should first push the
// current PC to the stack, so the subroutine can return later.
// (from: https://tobiasvl.github.io/blog/write-a-chip-8-emulator/#00ee-and-2nnn-subroutines)
void call_subroutine(chip_8 *c, uint16_t addr) {
    push(c, c->pc);
    c->pc = addr;
}

// 3XNN: skip if Vx is equal to nn
void skip_equal(chip_8 *c, uint8_t vx, uint16_t value) {
    if (c->V[vx] == value) {
        c->pc += 2;
    }
}

// 4XNN: skip if Vx is NOT equal to nn
void skip_not_equal(chip_8 *c, uint8_t vx, uint16_t value) {
    if (c->V[vx] != value) {
        c->pc += 2;
    }
}

// 5XY0: skip if Vx is equal to Vy
void skip_equal_reg(chip_8 *c, uint8_t vx, uint8_t vy) {
    if (c->V[vx] == c->V[vy]) {
        c->pc += 2;
    }
}

// 6XNN: set Vx to a value
// the title says it all, again. sets one of the regs to a provided value.
void set_reg(chip_8 *c, uint8_t vx, uint8_t value) {
    c->V[vx] = value;
}

// 7XNN: add value to Vx
// same as 6XNN, but adds instead of assigning.
void add_reg(chip_8 *c, uint8_t vx, uint8_t value) {
    c->V[vx] += value;
}

// 8XY0: set Vx to Vy
void set_reg_to_reg(chip_8 *c, uint8_t vx, uint8_t vy) {
    c->V[vx] = c->V[vy];
}

// 8XY1: Vx is set to bitwise OR of Vy
void bit_or_reg(chip_8 *c, uint8_t vx, uint8_t vy) {
    c->V[vx] |= c->V[vy];
    c->V[0xf] = 0;
}

// 8XY2: Vx is set to bitwise AND of Vy
void bit_and_reg(chip_8 *c, uint8_t vx, uint8_t vy) {
    c->V[vx] &= c->V[vy];
    c->V[0xf] = 0;
}

// 8XY3: Vx is set to bitwise XOR of Vy
void bit_xor_reg(chip_8 *c, uint8_t vx, uint8_t vy) {
    c->V[vx] ^= c->V[vy];
    c->V[0xf] = 0;
}

// 8XY4: set Vx to Vx + Vy
// additionally, if result is larger than 255, VF is set to 1
void add_reg_to_reg(chip_8 *c, uint8_t vx, uint8_t vy) {
    int result = c->V[vx] + c->V[vy];
    if (result > 0xff) {
        c->V[0xf] = 1;
    } else {
        c->V[0xf] = 0;
    }
    c->V[vx] = result;
}

// 8XY5: set Vx to Vx - Vy
// if Vx is larger than Vy, VF is set to 1, however if Vy is bigger, then VF is set to 0.
void sub_reg(chip_8 *c, uint8_t vx, uint8_t vy) {
    int result = c->V[vx] - c->V[vy];
    if (c->V[vx] > c->V[vy]) {
        c->V[0xf] = 1;
    } else {
        c->V[0xf] = 0;
    }
    c->V[vx] = result & 0xff;
}

// 8XY6: shift Vx one bit to right
void shift_reg_right(chip_8 *c, uint8_t vx, uint8_t vy) {
    c->V[vx] = c->V[vy];
    uint8_t bit = (c->V[vx]) & 1;
    c->V[vx] >>= 1;
    c->V[0xf] = bit;
}

// 8XY7: set Vx to Vy - Vx
// if Vx is larger than Vy, VF is set to 1, however if Vy is bigger, then VF is set to 0.
void sub_reg_rev(chip_8 *c, uint8_t vx, uint8_t vy) {
    int result = c->V[vy] - c->V[vx];
    if (c->V[vy] > c->V[vx]) {
        c->V[0xf] = 1;
    } else {
        c->V[0xf] = 0;
    }
    c->V[vx] = result & 0xff;
}

// 8XYE: shift Vx one bit to left
void shift_reg_left(chip_8 *c, uint8_t vx, uint8_t vy) {
    c->V[vx] = c->V[vy];
    c->V[vx] <<= 1;
    c->V[0xF] = (c->V[vx] >> 7) & 1;
}

// 9XY0: skip if Vx is NOT equal to Vy
void skip_not_equal_reg(chip_8 *c, uint8_t vx, uint8_t vy) {
    if (c->V[vx] != c->V[vy]) {
        c->pc += 2;
    }
}

// ANNN: set I to a given value
// the title. says. everything. AGAIN!
void set_index_reg(chip_8 *c, uint16_t value) {
    c->I = value;
}

// BNNN: jump to address plus V0
void jmp_addr_V0(chip_8 *c, uint16_t addr) {
    c->pc = addr + c->V[0];
}

// CXNN: generate a random number
// generate a random number (probably 0 to 255), binary AND it with nn and set Vx to it.
void num_gen(chip_8 *c, uint8_t vx, uint16_t value) {
    int rand_int = rand() % 256;
    c->V[vx] = rand_int & value;
}

// DXYN: draw sprite
// it is used to draw a “sprite” on the screen. Each sprite consists of 8-bit bytes, where each bit corresponds to a horizontal pixel; sprites are between 1 and
// 15 bytes tall. They’re drawn to the screen by treating all 0 bits as transparent, and all the 1 bits will “flip” the pixels in the locations of the screen that
// it’s drawn to. (You might recognize this as logical XOR.)
// (from: https://tobiasvl.github.io/blog/write-a-chip-8-emulator/)
void draw_sprite(chip_8 *c, uint8_t vx, uint8_t vy, uint8_t n) {
    int width = 8;
    c->V[0xF] = 0;
    int x_pos = (c->V[vx] & 0x3F);
    int y_pos = (c->V[vy] & 0x1F);
    for (int row = 0; row < n; row++) {
        // Get a row one of sprite data from the memory address in reg I (one byte per row)
        if (y_pos + row >= 32) break;
        uint8_t spriteData = (c->ram[c->I + row]);

        // for each 8 pixels/bits in this sprite row
        for (int col = 0; col < width; col++) {
            // if the current pixel in the sprite row is on,
            // and the screen pixel at X,Y is on, set pixel to 0, set VF to 1
            int y = (c->V[vy] + row) % 32;
            int x = (c->V[vx] + col) % 64;
            if (x_pos + col >= 64) break;
            if ((spriteData & 0x80) > 0) {
                if (x >= 64 || y >= 32) break;

                    if (c->display[x][y] == 1) {
                        c->V[0xF] = 1;
                    }
                    c->display[x][y] ^= 1;
                }
            // Point spriteData to the next bit
            spriteData <<= 1;
        }
    }
}

// EX9E: skip next instruction if key, that's stored in Vx is pressed
void skip_if_key_pressed(chip_8 *c, uint8_t vx) {
    if (c->keys[c->V[vx]] == 1) {
        c->pc += 2;
    }
}

// EX9E: skip next instruction if key, that's stored in Vx is NOT pressed
void skip_if_key_not_pressed(chip_8 *c, uint8_t vx) {
    if (c->keys[c->V[vx]] == 0) {
        c->pc += 2;
    }
}

// FX07: set Vx to delay timer
void set_reg_timer(chip_8 *c, uint8_t vx) {
    c->V[vx] = c->delay_timer;
}

// FX0A: halt until key press and store in Vx, and wait for the key to be released
void wait_for_key(chip_8 *c, uint8_t reg) {
    int last_key = current_key_pressed(c->keys);
    // no key pressed yet, move pc back
    if (last_key == -1) c->pc -= 2;
    // key was pressed, storing...
    c->V[reg] = last_key;
    // waiting for release
    if (current_key_pressed(c->keys) == -1) c->pc -= 2;
}

// FX15: set delay timer to Vx
void set_delay_timer(chip_8 *c, uint8_t vx) {
    c->delay_timer = c->V[vx];
}

// FX18: set sound timer to Vx
void set_sound_timer(chip_8 *c, uint8_t vx) {
    c->sound_timer = c->V[vx];
}

// FX1E: add Vx to index register
void add_reg_index(chip_8 *c, uint8_t vx) {
    c->I += c->V[vx];
}

// FX29: get font character
void get_font_char(chip_8 *c, uint8_t vx) {
    c->I = FONT_ADDRESS + c->V[vx] * 5;
}

// FX33: binary-coded decimal conversion
// takes the number from Vx and converts it to 3 decimal digits, storing them
// in memory at the address in the index register.
// for example, if Vx contains 156, (or 9C in hex), it would put the number 1 at the address in I,
// 5 in address I + 1, and 6 un address I + 2
void bcd(chip_8 *c, uint8_t vx) {
    int num = c->V[vx];
    int ones = num % 10;
    int tens = (num / 10) % 10;
    int huns = (num / 100) % 10;
    c->ram[c->I + 0] = huns & 0xff;
    c->ram[c->I + 1] = tens & 0xff;
    c->ram[c->I + 2] = ones & 0xff;
}

// FX55: store register data in memory
void store_reg(chip_8 *c, uint8_t x) {
    for (int i = 0; i <= x; i++) {
        c->ram[c->I + i] = c->V[i];
    }
    c->I += x + 1;
}

// FX65: load register data from memory
void load_reg(chip_8 *c, uint8_t x) {
    for (int i = 0; i <= x; i++) {
        c->V[i] = c->ram[c->I + i];
    }
    c->I += x + 1;
}
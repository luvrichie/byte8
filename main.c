#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "include/chip8.h"
#include "include/graphics.h"
#include "include/opcodes.h"
#include "include/input.h"

bool DEBUG = true;

void updateInput(chip_8 *c) {
    if (IsKeyDown(KEY_ONE)) {
        c->keys[0x1] = 1;
    } else if (IsKeyUp(KEY_ONE)) {
        c->keys[0x1] = 0;
    }
    if (IsKeyDown(KEY_TWO)) {
        c->keys[0x2] = 1;
    } else if (IsKeyUp(KEY_TWO)) {
        c->keys[0x2] = 0;
    }
    if (IsKeyDown(KEY_THREE)) {
        c->keys[0x3] = 1;
    } else if (IsKeyUp(KEY_THREE)) {
        c->keys[0x3] = 0;
    }
    if (IsKeyDown(KEY_FOUR)) {
        c->keys[0xc] = 1;
    } else if (IsKeyUp(KEY_FOUR)) {
        c->keys[0xc] = 0;
    }
    if (IsKeyDown(KEY_Q)) {
        c->keys[0x4] = 1;
    } else if (IsKeyUp(KEY_Q)) {
        c->keys[0x4] = 0;
    }
    if (IsKeyDown(KEY_W)) {
        c->keys[0x5] = 1;
    } else if (IsKeyUp(KEY_W)) {
        c->keys[0x5] = 0;
    }
    if (IsKeyDown(KEY_E)) {
        c->keys[0x6] = 1;
    } else if (IsKeyUp(KEY_E)) {
        c->keys[0x6] = 0;
    }
    if (IsKeyDown(KEY_R)) {
        c->keys[0xd] = 1;
    } else if (IsKeyUp(KEY_R)) {
        c->keys[0xd] = 0;
    }
    if (IsKeyDown(KEY_A)) {
        c->keys[0x7] = 1;
    } else if (IsKeyUp(KEY_A)) {
        c->keys[0x7] = 0;
    }
    if (IsKeyDown(KEY_S)) {
        c->keys[0x8] = 1;
    } else if (IsKeyUp(KEY_S)) {
        c->keys[0x8] = 0;
    }
    if (IsKeyDown(KEY_D)) {
        c->keys[0x9] = 1;
    } else if (IsKeyUp(KEY_D)) {
        c->keys[0x9] = 0;
    }
    if (IsKeyDown(KEY_F)) {
        c->keys[0xE] = 1;
    } else if (IsKeyUp(KEY_F)) {
        c->keys[0xE] = 0;
    }
    if (IsKeyDown(KEY_Z)) {
        c->keys[0xA] = 1;
    } else if (IsKeyUp(KEY_Z)) {
        c->keys[0xA] = 0;
    }
    if (IsKeyDown(KEY_X)) {
        c->keys[0x0] = 1;
    } else if (IsKeyUp(KEY_X)) {
        c->keys[0x0] = 0;
    }
    if (IsKeyDown(KEY_C)) {
        c->keys[0xB] = 1;
    } else if (IsKeyUp(KEY_C)) {
        c->keys[0xB] = 0;
    }
    if (IsKeyDown(KEY_V)) {
        c->keys[0xF] = 1;
    } else if (IsKeyUp(KEY_V)) {
        c->keys[0xF] = 0;
    }
}

void init(chip_8 *c) {
    memset(c->ram, 0, RAM_SIZE);
    memset(c->display, 0, DISPLAY_HEIGHT * DISPLAY_WIDTH);
    memset(c->stack, 0, STACK_SIZE);
    memset(c->V, 0, REGISTER_SIZE);
    memset(c->keys, 0, 16);

    memcpy(c->ram + FONT_ADDRESS, fontset, 80);
    printf("[*] loaded font-set into memory\n");

    c->pc = ROM_ADDRESS;
    c->I = 0;
    c->sp = 0;

    c->delay_timer = 0;
    c->sound_timer = 0;
    printf("[*] init finished\n");
}

void load(chip_8 *c, char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        fprintf(stderr, "[!] failed to open the selected file!\n");
        exit(1);
    }
    fseek(fp, 0L, SEEK_END);
    int size = ftell(fp);
    rewind(fp);
    if (size > RAM_SIZE - ROM_ADDRESS) {
        fprintf(stderr, "[!] file is too large to be copied\n");
        if (DEBUG == true) {
            printf("[DEBUG] exiting because size of binary is too large: %i", size);
        }
        exit(1);
    }
    fread(c->ram + ROM_ADDRESS, 1, size, fp);
    printf("[*] loaded ROM\n");
    fclose(fp);
}

void step(chip_8 *c) {
    uint8_t hi = c->ram[c->pc];
    uint8_t lo = c->ram[c->pc + 1];
    uint16_t full = (hi << 8) | lo;
    c->pc += 2;
    switch (hi >> 4) {
        case 0x0:
            if (0x00E0 == full) {
                clear_display(c);
            } else if (full == 0x00EE) {
                return_subroutine(c);
            } else {
                printf("[!] unimplemented opcode! 0x%04x\n", full);
            }
            break;
        case 0x1:
            jmp_addr(c, full & 0xfff);
            break;
        case 0x2:
            call_subroutine(c, full & 0xfff);
            break;
        case 0x3:
            skip_equal(c, hi & 0xf, lo);
            break;
        case 0x4:
            skip_not_equal(c, hi & 0xf, lo);
            break;
        case 0x5:
            skip_equal_reg(c, hi & 0xf, lo >> 4);
            break;
        case 0x6:
            set_reg(c, hi & 0xf, lo);
            break;
        case 0x7:
            add_reg(c, hi & 0xf, lo);
            break;
        case 0x8:
            switch (lo & 0xf) {
                case 0x0:
                    set_reg_to_reg(c, hi & 0xf, lo >> 4);
                    break;
                case 0x1:
                    bit_or_reg(c, hi & 0xf, lo >> 4);
                    break;
                case 0x2:
                    bit_and_reg(c, hi & 0xf, lo >> 4);
                    break;
                case 0x3:
                    bit_xor_reg(c, hi & 0xf, lo >> 4);
                    break;
                case 0x4:
                    add_reg_to_reg(c, hi & 0xf, lo >> 4);
                    break;
                case 0x5:
                    sub_reg(c, hi & 0xf, lo >> 4);
                    break;
                case 0x6:
                    shift_reg_right(c, hi & 0xf, lo >> 4);
                    break;
                case 0x7:
                    sub_reg_rev(c, hi & 0xf, lo >> 4);
                    break;
                case 0xe:
                    shift_reg_left(c, hi & 0xf, lo >> 4);
                    break;
                default:
                    printf("[!] unimplemented opcode! 0x%04x\n", full);
            }
            break;
        case 0x9:
            skip_not_equal_reg(c, hi & 0xf, lo >> 4);
            break;
        case 0xa:
            set_index_reg(c, full & 0xfff);
            break;
        case 0xb:
            jmp_addr_V0(c, full & 0xfff);
            break;
        case 0xc:
            num_gen(c, hi & 0xf, lo);
            break;
        case 0xd:
            draw_sprite(c, hi & 0xf, lo >> 4, lo & 0xf);
            break;
        case 0xe:
            switch (lo) {
                case 0x9e:
                    skip_if_key_pressed(c, hi & 0xf);
                    break;
                case 0xa1:
                    skip_if_key_not_pressed(c, hi & 0xf);
                    break;
                default:
                    printf("[!] unimplemented opcode! 0x%04x\n", full);
            }
            break;
        case 0xf:
            switch (lo) {
                case 0x07:
                    set_reg_timer(c, hi & 0xf);
                    break;
                case 0x0a:
                    wait_for_key(c, hi & 0xf);
                    break;
                case 0x15:
                    set_delay_timer(c, hi & 0xf);
                    break;
                case 0x18:
                    set_sound_timer(c, hi & 0xf);
                    break;
                case 0x1e:
                    add_reg_index(c, hi & 0xf);
                    break;
                case 0x29:
                    get_font_char(c, hi & 0xf);
                    break;
                case 0x33:
                    bcd(c, hi & 0xf);
                    break;
                case 0x55:
                    store_reg(c, hi & 0xf);
                    break;
                case 0x65:
                    load_reg(c, hi & 0xf);
                    break;
                default:
                    printf("[!] unimplemented opcode! 0x%04x\n", full);
            }
            break;
        default:
            printf("[!] unimplemented opcode! 0x%04x\n", full);
    }
}

int main(int argc, char *argv[]) {
    printf("[*] warming up...\n");
    if (argc < 2) {
        fprintf(stderr,
                "[!] no arguments specified, are you sure you ran the program like this:\n./program [path/to/ROM]\n");
        if (DEBUG == true) {
            printf("[DEBUG] exiting because argc is %i and argv is %s\n", argc, argv[1]);
        }
        exit(1);
    }

    chip_8 chip8;
    init(&chip8);

    load(&chip8, argv[1]);
    chip8.ram[0x1ff] = 5;

    init_gfx();
    SetTraceLogLevel(LOG_WARNING);

    const float fps = 60.0f;
    SetTargetFPS(fps);

    while (!WindowShouldClose()) {
        updateInput(&chip8);
        // 8 is the number of instructions to execute per frame
        if (chip8.delay_timer > 0) chip8.delay_timer--;
        for (int a = 0; a <= 8; a++) {
            step(&chip8);
        }
        draw_sprite_ray(&chip8);
    }
}

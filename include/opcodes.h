//
// Created by richie on 23/11/2022.
//

#ifndef BYTE8_OPCODES_H
#define BYTE8_OPCODES_H
void push(chip_8 *c, uint16_t value);
uint16_t pop(chip_8 *c);
void clear_display(chip_8 *c);
void return_subroutine(chip_8 *c);
void jmp_addr(chip_8 *c, uint16_t addr);
void call_subroutine(chip_8 *c, uint16_t addr);
void skip_equal(chip_8 *c, uint8_t vx, uint16_t value);
void skip_not_equal(chip_8 *c, uint8_t vx, uint16_t value);
void skip_equal_reg(chip_8 *c, uint8_t vx, uint8_t vy);
void set_reg(chip_8 *c, uint8_t vx, uint8_t value);
void add_reg(chip_8 *c, uint8_t vx, uint8_t value);
void set_reg_to_reg(chip_8 *c, uint8_t vx, uint8_t vy);
void bit_or_reg(chip_8 *c, uint8_t vx, uint8_t vy);
void bit_and_reg(chip_8 *c, uint8_t vx, uint8_t vy);
void bit_xor_reg(chip_8 *c, uint8_t vx, uint8_t vy);
void add_reg_to_reg(chip_8 *c, uint8_t vx, uint8_t vy);
void sub_reg(chip_8 *c, uint8_t vx, uint8_t vy);
void shift_reg_right(chip_8 *c, uint8_t vx, uint8_t vy);
void sub_reg_rev(chip_8 *c, uint8_t vx, uint8_t vy);
void shift_reg_left(chip_8 *c, uint8_t vx, uint8_t vy);
void skip_not_equal_reg(chip_8 *c, uint8_t vx, uint8_t vy);
void set_index_reg(chip_8 *c, uint16_t value);
void jmp_addr_V0(chip_8 *c, uint16_t addr);
void num_gen(chip_8 *c, uint8_t vx, uint16_t value);
void draw_sprite(chip_8 *c, uint8_t vx, uint8_t vy, uint8_t n);
void skip_if_key_pressed(chip_8 *c, uint8_t vx);
void skip_if_key_not_pressed(chip_8 *c, uint8_t vx);
void set_reg_timer(chip_8 *c, uint8_t vx);
void wait_for_key(chip_8 *c, uint8_t reg);
void set_delay_timer(chip_8 *c, uint8_t vx);
void set_sound_timer(chip_8 *c, uint8_t vx);
void add_reg_index(chip_8 *c, uint8_t vx);
void get_font_char(chip_8 *c, uint8_t vx);
void bcd(chip_8 *c, uint8_t vx);
void store_reg(chip_8 *c, uint8_t x);
void load_reg(chip_8 *c, uint8_t x);
#endif //BYTE8_OPCODES_H

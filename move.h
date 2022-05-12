#ifndef MOVE_H
#define MOVE_H

#include <stdio.h>
#include <stdbool.h>

void advance_distance(uint16_t speed, uint16_t dist);
void rotate_quarter_right(uint16_t speed);
void rotate_quarter_left(uint16_t speed);
void rotate_half(uint16_t speed);
bool is_moving(void);
void move_thd_start(void);
void stop_request(void);
void run_request(void);

#endif

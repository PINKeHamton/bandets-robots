#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int32_t motor_group_move(uint8_t *MotorArray, const int8_t voltage);

int32_t motor_group_move_relative(uint8_t *MotorArray, double position, const int8_t voltage);

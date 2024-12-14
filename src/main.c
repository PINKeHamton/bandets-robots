#include "../include/main.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#define ARRAY_SIZE(x) (sizeof((x)) / sizeof((x)[0]))

/* Motor Port Defs */

#define FRT_LEFT -1
#define END_LEFT -2

#define FRT_RIGHT 9
#define END_RIGHT 10

#define INTK 17

#define ARM 20

#define ELV_LEFT 18
#define ELV_RIGHT -19

/* ADI Port Defs */

#define PNU_CEN 'C'
#define PNU_DONKER 'B'

#define LED_G 'F'
#define LED_Y 'G'
#define LED_R 'H'

int32_t motor_group_move(uint8_t *motor_array, const int8_t voltage) {

    int count;

    uint8_t motor_array_size = ARRAY_SIZE(motor_array);

    for(count = 0; count < motor_array_size; count++)
        motor_move(motor_array[count], voltage);

    return 0;
}

void dev_init() {

/* MOTOR GEARINGS */
    motor_set_gearing(FRT_LEFT, MOTOR_GEAR_BLUE);
    motor_set_gearing(FRT_RIGHT, MOTOR_GEAR_BLUE);
    motor_set_gearing(END_LEFT, MOTOR_GEAR_BLUE);
    motor_set_gearing(END_RIGHT, MOTOR_GEAR_BLUE);
    motor_set_gearing(INTK, MOTOR_GEAR_GREEN);
    motor_set_gearing(ARM, MOTOR_GEAR_RED);
    motor_set_gearing(ELV_LEFT, MOTOR_GEAR_BLUE);
    motor_set_gearing(ELV_RIGHT, MOTOR_GEAR_BLUE);

/* ADI Setup */
    adi_port_set_config(PNU_CEN, E_ADI_DIGITAL_OUT);
    adi_port_set_config(PNU_DONKER, E_ADI_DIGITAL_OUT);

    adi_port_set_config(LED_G, E_ADI_DIGITAL_OUT);
    adi_port_set_config(LED_Y, E_ADI_DIGITAL_OUT);
    adi_port_set_config(LED_R, E_ADI_DIGITAL_OUT);

}

static bool flip = false;

void on_left_button() {

    flip = !flip;

    if(flip) {
       lcd_set_text(3, "Blue Autonomous");

    } else {
        lcd_clear_line(3);
        lcd_set_text(3, "Red Autonomous");
    }

}

void autonomous() {



}

void initialize() {

    lcd_initialize();

    //lcd_set_background_color(COLOR_BLACK);
    //lcd_set_text_color(COLOR_WHITE);

    lcd_register_btn2_cb(on_left_button);

}

void opcontrol() {

    /* Run Func */

    dev_init();

    FILE* logfile = NULL;

    uint8_t MA_Left[] = {FRT_LEFT,END_LEFT};
    uint8_t MA_Right[] = {FRT_RIGHT,END_RIGHT};

    uint8_t MA_ELV[] = {ELV_LEFT,ELV_RIGHT};

    /* END Run Func */
    while (true) {

        int linear = controller_get_analog(CONTROLLER_MASTER, ANALOG_LEFT_Y) * 0.8;
        int strafe = controller_get_analog(CONTROLLER_MASTER, ANALOG_RIGHT_X) * 0.8;
        int left = linear;
        int right = linear;

        if(strafe <0 ) {
            left+=strafe;
        } else if (strafe >0) {
            right-=strafe;
        }

        if(controller_get_digital(CONTROLLER_MASTER, DIGITAL_UP)) {
            motor_move(ARM, 127 * 0.8);
        } else if (controller_get_digital(CONTROLLER_MASTER, DIGITAL_DOWN)) {
            motor_move(ARM, -127 * 0.8);
        } else {
            motor_move(ARM, 0);
        }

        motor_group_move(MA_Left, left);
        motor_group_move(MA_Right, right);


        int motor_left_val = motor_get_voltage(FRT_LEFT);
        int motor_right_val = motor_get_voltage(FRT_RIGHT);

        lcd_print(0, "\t\tDrive DEBUG\n");
        lcd_print(1, "int left: %d\t int right: %d\n", motor_left_val, motor_right_val); 

        int lift = controller_get_analog(CONTROLLER_PARTNER, ANALOG_LEFT_Y) * 0.8;

        motor_group_move(MA_ELV, -lift);

        static bool INTKbool = false;

        static bool DONKER_tog = false;
        static bool CEN_tog = false;

        if(controller_get_digital_new_press(CONTROLLER_MASTER, DIGITAL_R1)) {
                adi_digital_write(PNU_CEN, CEN_tog);
                CEN_tog = !CEN_tog;
            }

        if(controller_get_digital_new_press(CONTROLLER_MASTER, DIGITAL_R2)) {
                adi_digital_write(PNU_DONKER, DONKER_tog);
                DONKER_tog = !DONKER_tog;
            }

        if(controller_get_digital_new_press(CONTROLLER_PARTNER, DIGITAL_A)) {
                INTKbool = !INTKbool;
        }

        if (INTKbool) {
            motor_move(INTK, -127 * 0.8);
        } else if (!INTKbool) {
            motor_move(INTK, 0);
        }

        delay(20);

    }

}

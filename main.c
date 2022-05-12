#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ch.h"
#include "hal.h"
#include "memory_protection.h"
#include <usbcfg.h>
#include <main.h>
#include <chprintf.h>
#include <motors.h>
#include <audio/microphone.h>
#include <move.h>

#include <audio_processing.h>
#include <fft.h>
#include <communications.h>
#include <arm_math.h>

enum State{LISTEN, GO, LEFT, RIGHT, BACK, MOVING};

static void serial_start(void)
{
	static SerialConfig ser_cfg = {
	    115200,
	    0,
	    0,
	    0,
	};

	sdStart(&SD3, &ser_cfg); // UART3.
}

static void timer12_start(void){
    //General Purpose Timer configuration   
    //timer 12 is a 16 bit timer so we can measure time
    //to about 65ms with a 1Mhz counter
    static const GPTConfig gpt12cfg = {
        1000000,        /* 1MHz timer clock in order to measure uS.*/
        NULL,           /* Timer callback.*/
        0,
        0
    };

    gptStart(&GPTD12, &gpt12cfg);
    //let the timer count to max value
    gptStartContinuous(&GPTD12, 0xFFFF);
}

int main(void) {
    halInit();
    chSysInit();
    mpu_init();

    //starts the serial communication
    serial_start();
    //starts the USB communication
    usb_start();
    //starts timer 12
    timer12_start();
    //inits the motors
    motors_init();
    move_thd_start();

    enum State current_state = BACK;

    /* Infinite loop. */
    while (1) {
    	switch(current_state) {
    		case LISTEN:
    			continue;
    		case GO:
    			advance_distance(5,20);
    			current_state = MOVING;
    			continue;
    		case LEFT:
    			rotate_quarter_left(5);
				current_state = MOVING;
    			continue;
    		case RIGHT:
    			rotate_quarter_right(5);
				current_state = MOVING;
				continue;
    		case BACK:
    			rotate_half(5);
    			current_state = MOVING;
    			continue;
    		case MOVING:
    			if(!is_moving()) {
    				current_state = LISTEN;
    			}
    			continue;
    		default:
    			continue;
    	}
    	chThdSleepMilliseconds(100);
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}

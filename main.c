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
#include <classifier.h>

#include <audio_processing.h>
#include <fft.h>
#include <communications.h>
#include <arm_math.h>

typedef enum {LISTEN, GO, LEFT, RIGHT, BACK, MOVING} State;

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



int main(void)
{

    halInit();
    chSysInit();
    mpu_init();

    messagebus_init(&bus, &bus_lock, &bus_condvar);

    serial_start();
    usb_start();
    timer12_start();
    motors_init();
    move_thd_start();
    classifier_init();
    mic_start(&processAudioData);

    State current_state = LISTEN;
    Command current_command = CMD_NOISE;
    /* Infinite loop. */
    while (1) {
    	switch(current_state) {
    		case LISTEN:
                listen();
                float* samples_vect_out = get_audio_buffer_ptr(FRONT_OUTPUT);
                classifier_predict(samples_vect_out, &current_command);
            	chprintf((BaseSequentialStream *)&SD3," %.2f \r\n", current_command);
                switch(current_command) {
                	case CMD_NOISE:
                		continue;
                	case CMD_GO:
                		current_state = GO;
                		continue;
                	case CMD_RIGHT:
                		current_state = RIGHT;
						continue;
                	case CMD_LEFT:
                		current_state = LEFT;
						continue;
                	case CMD_BACK:
                		current_state = BACK;
						continue;
                	default:
                		continue;
                }
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

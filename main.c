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
#include <distance.h>

#include <audio_processing.h>
#include <fft.h>
#include <communications.h>
#include <arm_math.h>

#define MOVE_SPEED		5	//[cm/s]
#define GO_DISTANCE		20	//[cm]
#define QUARTER_ANGLE	90	//[deg]
#define	HALF_ANGLE		180	//[deg]

#define MAIN_SLEEP_TIME	100	//[ms]

messagebus_t bus;
MUTEX_DECL(bus_lock);
CONDVAR_DECL(bus_condvar);

typedef enum {LISTEN, GO, LEFT, RIGHT, BACK, MOVING} STATE;

static void serial_start(void)
{
	static SerialConfig ser_cfg = {
	    115200,
	    0,
	    0,
	    0,
	};

	sdStart(&SD3, &ser_cfg);
}

int main(void)
{
	//Initializations
    halInit();
    chSysInit();
    mpu_init();
    messagebus_init(&bus, &bus_lock, &bus_condvar);
    serial_start();
    usb_start();
	proximity_start();
	calibrate_ir();
    motors_init();
    move_thd_start();
    classifier_init();
    mic_start(&processAudioData);
    IrSens_start();

    STATE current_state = LISTEN;
    COMMAND current_command = CMD_NOISE;
    /* Infinite loop. */
    while (1) {
    	switch(current_state) {
    		case LISTEN:
    			//Detection of words
    			current_command = CMD_NOISE;
    		    chThdSetPriority(NORMALPRIO+2);
                listen();
                float* samples_vect_out = get_audio_buffer_ptr(FRONT_OUTPUT);
                classifier_predict(samples_vect_out, &current_command);
                chThdSetPriority(NORMALPRIO);

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
    		//Reactions to detected words
    		case GO:
    			if(object_detected()) {
    				current_state = LISTEN;
    				continue;
    			}
    			advance(MOVE_SPEED,GO_DISTANCE);
    			current_state = MOVING;
    			continue;
    		case LEFT:
    			rotate(-MOVE_SPEED,QUARTER_ANGLE);
				current_state = MOVING;
    			continue;
    		case RIGHT:
    			rotate(-MOVE_SPEED,QUARTER_ANGLE);
				current_state = MOVING;
				continue;
    		case BACK:
    			rotate(MOVE_SPEED,HALF_ANGLE);
    			current_state = MOVING;
    			continue;
    		case MOVING:
    			if(current_command == CMD_GO && object_detected()){
    				stop_request();
				}
    			if(!is_moving()) {
    				clear_stop_request();
    				current_state = LISTEN;
    			}
    			continue;
    		default:
    			continue;
    	}
    	chThdSleepMilliseconds(MAIN_SLEEP_TIME);
    }
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}

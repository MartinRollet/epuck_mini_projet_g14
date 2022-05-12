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
#include <classifier.h>

#include <audio_processing.h>
#include <fft.h>
#include <communications.h>
#include <arm_math.h>

typedef enum {LISTEN, GO, LEFT, RIGHT, BACK} State;

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

    serial_start();
    usb_start();
    timer12_start();
    motors_init();
    classifier_init();
    mic_start(&processAudioData);

    State current_state = LISTEN;
    Command current_command = N;

    while (1) {
    	switch(current_state) {
			case LISTEN:
				listen();
				float* samples_vect_out = get_audio_buffer_ptr(FRONT_OUTPUT);
				classifier_predict(samples_vect_out, &current_command);
				chThdSleepMilliseconds(1000);
				break;

			case GO:
				break;
			case LEFT:
				break;
			case RIGHT:
				break;
			case BACK:
				break;
			default:
				break;
		}

	}
}

#define STACK_CHK_GUARD 0xe2dee396
uintptr_t __stack_chk_guard = STACK_CHK_GUARD;

void __stack_chk_fail(void)
{
    chSysHalt("Stack smashing detected");
}

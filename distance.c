/*
 * distance.c
 *
 *  Created on: 12 mai 2022
 *      Author: Martin
 */

#include <distance.h>
#include <stdlib.h>
#include <main.h>

#include <chprintf.h>


#define USED_IR_SENSOR 1
#define PROXIMITY_VALUE	2000

static bool detected = false;

bool object_detected(void) {
	return detected;
}

// --- IR SENSOR THREAD ---

static THD_WORKING_AREA(waIrSens, 128);
static THD_FUNCTION(IrSens, arg) {
	chRegSetThreadName(__FUNCTION__);
	(void)arg;
	while(1) {
	    chprintf((BaseSequentialStream *)&SD3,"Capteur Dist: %D\r\n", get_calibrated_prox(0));
//		if(get_calibrated_prox(1) > PROXIMITY_VALUE){
//			detected = true;
//		} else {
//			detected = false;
//		}

		chThdSleepMilliseconds(300);
	}
}

void IrSens_start(void){
	chThdCreateStatic(waIrSens, sizeof(waIrSens), NORMALPRIO+1, IrSens, NULL);
}

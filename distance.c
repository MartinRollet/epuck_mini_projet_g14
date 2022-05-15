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


#define USED_IR_SENSOR 0
#define PROXIMITY_VALUE	40

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
		if(get_calibrated_prox(USED_IR_SENSOR) > PROXIMITY_VALUE){
			detected = true;
		} else {
			detected = false;
		}
		chThdSleepMilliseconds(100);
	}
}

void IrSens_start(void){
	chThdCreateStatic(waIrSens, sizeof(waIrSens), NORMALPRIO+1, IrSens, NULL);
}

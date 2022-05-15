/*
 * distance.c
 *
 *  Created on: 12 mai 2022
 *      Author: Martin
 */

#include <distance.h>
#include <stdlib.h>
#include <main.h>

#define USED_IR_SENSOR		0
#define PROXIMITY_VALUE		40
#define DISTANCE_SLEEP_TIME	100	//[ms]

static bool detected = false;

// --- PUBLIC FUNCTIONS ---

bool object_detected(void) {
	return detected;
}

// --- IR SENSOR THREAD ---

static THD_WORKING_AREA(waIrSens, 128);
static THD_FUNCTION(IrSens, arg) {
	chRegSetThreadName(__FUNCTION__);
	(void)arg;
	while(1) {
		//Check if something is detected close to the robot
		if(get_calibrated_prox(USED_IR_SENSOR) > PROXIMITY_VALUE){
			detected = true;	//Update detection status
		} else {
			detected = false;	//Update detection status
		}
		chThdSleepMilliseconds(DISTANCE_SLEEP_TIME);
	}
}

void IrSens_start(void){
	chThdCreateStatic(waIrSens, sizeof(waIrSens), NORMALPRIO+1, IrSens, NULL);
}

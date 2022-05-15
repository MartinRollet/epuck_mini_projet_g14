/*
 * distance.h
 *
 *  Created on: 12 mai 2022
 *      Author: Martin
 */

#include <stdbool.h>
#include <sensors/proximity.h>

#ifndef DISTANCE_H_
#define DISTANCE_H_

/*!
 * @function	object_detected
 * @abstract	Check if an object has been detected.
 * @discussion	Check if an object is cirrently in front of the robot,
 * 				using front IR sensor.
 * @result		true if object detected, false otherwise.
*/
bool object_detected(void);

/*!
 * @function	IrSens_start
 * @abstract	Initialize thread for distance sensor.
 * @discussion	Start the IrSens thread, which is called every
 * 				100 milliseconds and have normal priority +1.
*/
void IrSens_start(void);

#endif /* DISTANCE_H_ */

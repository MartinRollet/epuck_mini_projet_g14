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

bool object_detected(void);
void IrSens_start(void);

#endif /* DISTANCE_H_ */

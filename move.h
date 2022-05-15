#ifndef MOVE_H
#define MOVE_H

#include <stdio.h>
#include <stdbool.h>

/*!
 * @function	advance
 * @abstract	Make the robot move forward for a given distance.
 * @discussion	This function enables distance and speed control.
 * @param		speed	Speed in [cm/s] for the movement,
 * 						positive value means forward.
 * @param		dist 	Distance to run in [cm].
*/
void advance(int16_t speed, uint16_t dist);

/*!
 * @function	rotate
 * @abstract	Make the robot rotate for a given angle.
 * @discussion	This function enables angle and speed control.
 * @param		speed	Speed in [cm/s] for the movement,
 * 						positive value means clockwise.
 * @param		angle 	Angle in [deg] to rotate.
*/
void rotate(int16_t speed, uint16_t angle);

/*!
 * @function	is_moving
 * @abstract	Check if the robot is currently moving.
 * @discussion	Give the status of the step motors.
 * @result		true if moving, false otherwise.
*/
bool is_moving(void);

/*!
 * @function	stop_request
 * @abstract	Send a request to stop the motors.
 * @discussion	Set the stop request flag in this module,
 * 				motors will stop at next thread call.
*/
void stop_request(void);

/*!
 * @function	move_thd_start
 * @abstract	Initialize thread fro movements.
 * @discussion	Start the move_thd, which is called every
 * 				100 milliseconds and have normal priority.
*/
void move_thd_start(void);

#endif

#include <move.h>
#include <motors.h>
#include <stdlib.h>
#include <main.h>

#define STEPS_PER_TURN	1000	//[steps] for 1 turn of the motor
#define WHEEL_PERIMETER	13		//[cm]
#define WHEEL_DISTANCE	5.25	//[cm]
#define PI				3.1416
#define FULL_ANGLE		360		//[deg]

#define MOVE_SLEEP_TIME	100		//[ms]

static bool move_active = false;
static bool stop_requested = false;
static uint16_t move_goal = 0;

// --- INTERNAL FUNCTIONS ---

/*!
 * @function	reset_motors_pos
 * @abstract	Reset position of step motors.
 * @discussion	Reset the position counter of both step motors to enable
 * 				correct position control.
*/
void reset_motors_pos(void) {
	left_motor_set_pos(0);
	right_motor_set_pos(0);
	return;
}

/*!
 * @function	speed_steps
 * @abstract	Convert speed units.
 * @discussion	Convert speed from [cm/s] to [motorsteps/s].
 * @param		speed	Speed in [cm/s]
 * @result		Converted speed in [motorsteps/s].
*/
int16_t speed_steps(int16_t speed) {
	return STEPS_PER_TURN*speed/WHEEL_PERIMETER;
}

/*!
 * @function	dist_steps
 * @abstract	Convert distance units.
 * @discussion	Convert distance from [cm] to [motorsteps].
 * @param		distance	Distance in [cm]
 * @result		Converted distance in [motorsteps].
*/
uint16_t dist_steps(uint16_t dist) {
	return STEPS_PER_TURN*dist/WHEEL_PERIMETER;
}

// --- PUBLIC FUNCTIONS ---

void advance(int16_t speed, uint16_t dist) {
	reset_motors_pos();
	int16_t speed_step = speed_steps(speed);
	left_motor_set_speed(speed_step);
	right_motor_set_speed(speed_step);
	move_goal = dist_steps(dist);
	move_active = true;
}

void rotate(int16_t speed, uint16_t angle) {
	reset_motors_pos();
	int16_t speed_step = speed_steps(speed);
	left_motor_set_speed(speed_step);
	right_motor_set_speed(-speed_step);
	move_goal = dist_steps(PI*WHEEL_DISTANCE*angle/FULL_ANGLE);
	move_active = true;
}

bool is_moving(void) {
	return move_active;
}

void stop_request(void) {
	stop_requested = true;
}

void clear_stop_request(void){
	stop_requested = false;
}

// --- MOVEMENTS THREAD ---

static THD_WORKING_AREA(waMoveThd, 256);
static THD_FUNCTION(MoveThd, arg) {
	chRegSetThreadName(__FUNCTION__);
	(void)arg;
	while(1) {
		if(move_active){
			//Stop the motors if requested
			if(stop_requested) {
				left_motor_set_speed(0);
				right_motor_set_speed(0);
				move_active = false;
			}
			//Check if goal position is reached
			else {
				bool left_reached = abs(left_motor_get_pos()) > abs(move_goal);
				bool right_reached = abs(right_motor_get_pos()) > abs(move_goal);
				//Stop motors if goal position reached
				if(left_reached) {
					left_motor_set_speed(0);
				}
				if(right_reached) {
					right_motor_set_speed(0);
				}
				//Update motor status
				if(left_reached && right_reached) {
					move_active = false;
				}
			}
		}
		chThdSleepMilliseconds(MOVE_SLEEP_TIME);
	}
}

void move_thd_start(void){
	chThdCreateStatic(waMoveThd, sizeof(waMoveThd), NORMALPRIO, MoveThd, NULL);
}

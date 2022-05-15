#include <move.h>
#include <motors.h>
#include <stdlib.h>
#include <main.h>

#define STEPS_PER_TURN	1000	//[steps] for 1 turn of the motor
#define WHEEL_PERIMETER	13		//[cm]
#define WHEEL_DISTANCE	5.25	//[cm]
#define PI				3.1416

static bool move_active = false;
static bool stop_requested = false;
static uint16_t move_goal = 0;

// --- INTERNAL FUNCTIONS ---

void reset_motors_pos(void) {
	left_motor_set_pos(0);
	right_motor_set_pos(0);
	return;
}

//bool position_reached(int dist) {
//	bool right_pos_reached = false;
//	bool left_pos_reached = false;
//	if(abs(right_motor_get_pos()) > abs(dist)) {
//		right_pos_reached = true;
//		right_motor_set_speed(0);
//	}
//	if(abs(left_motor_get_pos()) > abs(dist)) {
//		left_pos_reached = true;
//		left_motor_set_speed(0);
//	}
//	if(right_pos_reached && left_pos_reached) {
//		return true;
//	}
//	return false;
//}

int speed_steps(uint16_t speed) {
	return STEPS_PER_TURN*speed/WHEEL_PERIMETER;
}

int dist_steps(uint16_t dist) {
	return STEPS_PER_TURN*dist/WHEEL_PERIMETER;
}

// --- PUBLIC FUNCTIONS ---

void advance_distance(uint16_t speed, uint16_t dist) {
	reset_motors_pos();
	uint16_t speed_step = speed_steps(speed);
	left_motor_set_speed(speed_step);
	right_motor_set_speed(speed_step);
	move_goal = dist_steps(dist);
	move_active = true;
}

void rotate_quarter_right(uint16_t speed) {
	reset_motors_pos();
	uint16_t speed_step = speed_steps(speed);
	left_motor_set_speed(speed_step);
	right_motor_set_speed(-speed_step);
	move_goal = dist_steps(PI*WHEEL_DISTANCE/4);
	move_active = true;
}

void rotate_quarter_left(uint16_t speed) {
	reset_motors_pos();
	uint16_t speed_step = speed_steps(speed);
	left_motor_set_speed(-speed_step);
	right_motor_set_speed(speed_step);
	move_goal = dist_steps(PI*WHEEL_DISTANCE/4);
	move_active = true;
}

void rotate_half(uint16_t speed) {
	reset_motors_pos();
	uint16_t speed_step = speed_steps(speed);
	left_motor_set_speed(-speed_step);
	right_motor_set_speed(speed_step);
	move_goal = dist_steps(PI*WHEEL_DISTANCE/2);
	move_active = true;
}


bool is_moving(void) {
	return move_active;
}

void stop_request(void) {
	stop_requested = true;
}

void run_request(void){
	stop_requested = false;
}

// --- MOVEMENTS THREAD ---

static THD_WORKING_AREA(waMoveThd, 256);
static THD_FUNCTION(MoveThd, arg) {
	chRegSetThreadName(__FUNCTION__);
	(void)arg;
	while(1) {
			if(move_active){
				if(stop_requested) {
					left_motor_set_speed(0);
					right_motor_set_speed(0);
					move_active = false;
					stop_requested = false;
				}
				else {
					bool left_reached = abs(left_motor_get_pos()) > abs(move_goal);
					bool right_reached = abs(right_motor_get_pos()) > abs(move_goal);
					if(left_reached) {
						left_motor_set_speed(0);
					}
					if(right_reached) {
						right_motor_set_speed(0);
					}
					if(left_reached && right_reached) {
						move_active = false;
					}
				}
			}
		chThdSleepMilliseconds(100);
	}
}

void move_thd_start(void){
	chThdCreateStatic(waMoveThd, sizeof(waMoveThd), NORMALPRIO, MoveThd, NULL);
}

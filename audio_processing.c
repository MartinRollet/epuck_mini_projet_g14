#include "ch.h"
#include "hal.h"
#include <main.h>
#include <usbcfg.h>
#include <chprintf.h>

#include <audio/microphone.h>
#include <audio_processing.h>
#include <communications.h>
#include <fft.h>
#include <arm_math.h>

//semaphore
static BSEMAPHORE_DECL(sendToComputer_sem, TRUE);

#define LIMIT_NOISE		20	//we don't sample noise
#define LIMIT_MAX		150	//we don't analyze if the is signal too weak
#define LIMIT_ENERGY	600	//we don't analyze if the is signal's normalized energy is too low

static float micFront_input[SAMPLE_SIZE*2] = {0,};
static float micFront_out[SAMPLE_SIZE] = {0,};

static float max = 0;
static float sampled = 0;
static uint16_t nb_samples = 0;
static bool reader = false;
static bool ctrl_trig = 0;


bool energy(float* vect, int16_t size){
	float sum_total = 0;
	for(uint16_t i = 0; i < size; i++){
		if(vect[i] > 0){
				sum_total += vect[i];
		}else{
			sum_total += -vect[i];
		}
	}

	if(sum_total > LIMIT_ENERGY){
		return false;
	}
	return true;
}


void standardize(float* vect, int16_t size){
	float max = 0;
	for(uint16_t i = 0; i < size; i++){
		if((vect[i]) > max){
			max = (vect[i]) ;
		}
	}

	for(uint16_t i = 0; i < size; i++){
		vect[i] = vect[i]/max;
	}
}


void processAudioData(int16_t *data, uint16_t num_samples){
	if(ctrl_trig){
		palSetPad(GPIOB, GPIOB_LED_BODY);
		for(uint16_t i = 0 ; i < num_samples ; i+=4){

			if(nb_samples >= (SAMPLE_SIZE*2)){
				if(max > LIMIT_MAX){
					reader = false;
					max = 0;
					break;
				}else{
					nb_samples = 0;
				}
			}

			sampled = (float)data[i + MIC_FRONT];
			if(reader){
					if(sampled > max){
						max = sampled;
					}
					micFront_input[nb_samples] = sampled;
					nb_samples++;

			}else if(sampled > LIMIT_NOISE){
				reader = true;
			}
		}

		if(nb_samples >= (SAMPLE_SIZE*2)){
			nb_samples = 0;
			standardize(micFront_input, SAMPLE_SIZE*2);
			if(!energy(micFront_input, SAMPLE_SIZE*2)){
				ctrl_trig = false;
				palClearPad(GPIOB, GPIOB_LED_BODY);
				doFFT_optimized(SAMPLE_SIZE, micFront_input);
				arm_cmplx_mag_f32(micFront_input, micFront_out, SAMPLE_SIZE);
				standardize(micFront_out, SAMPLE_SIZE);
				chBSemSignal(&sendToComputer_sem);
			}
		}
	}
}


void listen(void){
	ctrl_trig = 1;
	chBSemWait(&sendToComputer_sem);
}


float* get_audio_buffer_ptr(BUFFER_NAME_t name){

	if (name == FRONT_INPUT){
		return micFront_input;
		}
	else if (name == FRONT_OUTPUT){
		return micFront_out;
	}
	else{
		return NULL;
	}
}

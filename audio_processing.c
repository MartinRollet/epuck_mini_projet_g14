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

static BSEMAPHORE_DECL(Listen_sem, TRUE);	//Listen_sem Semaphore

#define LIMIT_NOISE		20		//we don't sample noise
#define LIMIT_MAX		150		//we don't analyze if the is signal's maximum value is too low
#define LIMIT_ENERGY	600		//we don't analyze if the is signal's normalized energy is too low

static float micFront_input[SAMPLE_SIZE*2] = {0,};	//Input buffer containing raw samples
static float micFront_out[SAMPLE_SIZE] = {0,}; 		//Output buffer contatinng final processed samples

static float max = 0;			//signals maximal value
static float sampled = 0; 		//intermediate variable
static uint16_t nb_samples = 0; // number of saved samples
static bool reader = false;		//control variable for the start of sample saving period
static bool ctrl_trig = false;	//control variable for the control of listening period

// --- INTERNAL FUNCTIONS ---

/*
 * @function	energy
 * @abstract	calculates the average energy of the signal
 * @param		*vect		Buffer for which the energy has to be calculated
 * @param		size		size of the buffer
 * @reslut		returns true if the average energy is > LIMIT_ENERGY, otherwise false
 */
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

/*
 * @function	normalize
 * @abstract	normalizes the given vector by its maximum value
 * @param		*vect		buffer that has to be normalized
 * @param		size		size of the buffer
 */
void normalize(float* vect, int16_t size){
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

// --- PUBLIC FUNCTIONS ---

void processAudioData(int16_t *data, uint16_t num_samples){
	if(ctrl_trig){
		palSetPad(GPIOB, GPIOB_LED_BODY);
		for(uint16_t i = 0 ; i < num_samples ; i+=4){
			if(nb_samples >= (SAMPLE_SIZE*2)){
				// exit the loop if enough samples are saved & the max of amplitude > LIMIT_MAX
				if(max > LIMIT_MAX){
					reader = false;
					max = 0;
					break;
				}else{
					nb_samples = 0;
				}
			}
			// start recording if a sample > LIMIT_NOISE (static noise) is received
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
			// Samples are normalized
			normalize(micFront_input, SAMPLE_SIZE*2);
			// if the sample energy is above adequately high
			if(!energy(micFront_input, SAMPLE_SIZE*2)){
				ctrl_trig = false;
				palClearPad(GPIOB, GPIOB_LED_BODY);
				// FFT is calculated and normalized
				doFFT_optimized(SAMPLE_SIZE, micFront_input);
				arm_cmplx_mag_f32(micFront_input, micFront_out, SAMPLE_SIZE);
				normalize(micFront_out, SAMPLE_SIZE);
				// we signal the semaphore that the listening has ended
				chBSemSignal(&Listen_sem);
			}
		}
	}
}

void listen(void){
	ctrl_trig = true;
	chBSemWait(&Listen_sem);
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

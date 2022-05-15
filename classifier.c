#include <main.h>
#include <classifier.h>
#include <audio_processing.h>
#include <arm_math.h>

// contains the wrapper functions for the neural net implementation
#include <nnom.h>
// contains the weights and the model architecture of the quantized network
#include "weights.h"

static int8_t input[SAMPLE_SIZE] = {0,}; //Quantized Input buffer for the model
static nnom_model_t* model;	//model object variable

/*
 * @function	quantize_data
 * @abstract	quantizes a float buffer in int8_t format
 * @discussion	is called before the classification in order to have the
 * 				appropriate input for our model
 * @param		*din		buffer in float format
 * @param		*dout		quantized signal in int8_t format
 * @param		size		buffer's size
 */
void quantize_data(float* din, int8_t* dout, uint16_t size){
	for(uint16_t i = 0; i < size; i++){
		dout[i] = (int8_t)(127 * (round(din[i]* 128)/128));
	}
}

void classifier_init(void){
	model = nnom_model_create();
	return;
}

void classifier_predict(float *data, COMMAND *cmd){
	uint32_t predic_label = 0;
	float prob = 0;
	quantize_data(data, input, SAMPLE_SIZE);
	memcpy(&nnom_input_data,  &input, sizeof(nnom_input_data));
	model_run(model);
	nnom_predict(model, &predic_label, &prob);
    *cmd = predic_label;
    return;
}


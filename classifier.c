#include <main.h>

#include <chprintf.h>
#include <classifier.h>
#include <audio_processing.h>
#include <arm_math.h>

#include "nnom.h"
#include "weights.h"


const char label_name[][10] = {"Noise", "Go", "Right", "Left", "Back"};

static int8_t input[SAMPLE_SIZE] = {0,};
static nnom_model_t* model;

void classifier_init(void){
	model = nnom_model_create();
	return;
}


void quantize_data(float* din, int8_t* dout, uint16_t size){
	for(uint16_t i = 0; i < size; i++){
//				if(din[i] > 0.4){
//					din[i] = 0.4;
//				}
		dout[i] = (int8_t)(127 * (round(din[i]* 128)/128));
	}
}


void classifier_predict(float *data, Command *cmd){
	uint32_t predic_label = 0;
	float prob = 0;

	quantize_data(data, input, SAMPLE_SIZE);
	memcpy(&nnom_input_data,  &input, sizeof(nnom_input_data));
	model_run(model);
	nnom_predict(model, &predic_label, &prob);

	chprintf((BaseSequentialStream *)&SD3,"%s \r\n", (char*)&label_name[predic_label]);
    chprintf((BaseSequentialStream *)&SD3,"Predicted label: %d\r\n", predic_label);
    chprintf((BaseSequentialStream *)&SD3,"Probability: %.2f \r\n", (prob));

    *cmd = predic_label;
    return;
}


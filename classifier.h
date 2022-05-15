#ifndef CLASSIFIER_H
#define CLASSIFIER_H

/*
 * the classes of possible command signals classified by the model
 */
typedef enum {CMD_NOISE, CMD_GO, CMD_RIGHT, CMD_LEFT, CMD_BACK} COMMAND;

/*
 * @function	classifier_init
 * @abstract	initializes the classification static object "Model"
 * @discussion	is called once to initialize the neural net object "Model" by importing
 * 				the characteristic information contained in the "weights.h" file and
 * 				reconstructing the quantized network layer by layer
 */
void classifier_init(void);

/*
 * @function	classifier_predict
 * @abstract	in charge of performing the classification of the data
 * @discussion	is called after the listening step is finished and a adequate FFT buffer
 * 				is prepared. It quantizes the signal buffer into int8_t format and passes
 * 				it to the classification model.
 * @param		*data		normalized FFT buffer in float format
 * @param		*cmd		COMMAND enum type pointer which is used to return the
 * 							classification result
 */
void classifier_predict(float *data, COMMAND *cmd);

#endif /* CLASSIFIER_H */

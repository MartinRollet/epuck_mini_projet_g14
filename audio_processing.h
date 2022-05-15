#ifndef AUDIO_PROCESSING_H
#define AUDIO_PROCESSING_H

#define SAMPLE_SIZE	4096 //Dim of output buffer = Dim of input buffer divided by 2

/*
 * Retrieve the Input or Output buffer pointer @function through get_audio_buffer_ptr
 */
typedef enum {FRONT_INPUT, FRONT_OUTPUT} BUFFER_NAME_t;

/*
 * @function	processAudioData
 * @abstract	in charge of signal receiving and treatement
 * @discussion	Is called in loop to receive samples of over 10ms, ignores faulty signals
 * 				or noise by looking at the amplitude and their energy using the internal
 * 				energy function. Calculates noramilzed FFT of the of the final signal.
 * @param		*data		Buffer containing 4 times 160 samples. the samples are sorted
 * 							by micro so we have [micRight1, micLeft1, micBack1, micFront1,
 * 							micRight2, etc...]
 * @param		num_samples	Tells how many data we get in total (should always be 640)
 * @note		this process is coupled with Listen_sem Semaphore in order to insure correct
 * 				sampling
 */
void processAudioData(int16_t *data, uint16_t num_samples);

/*
 * @function	listen
 * @abstract	Insures correct sampling period by activating the Listen_sem Semaphore &
 * 				control variable ctrl_trig
 */
void listen(void);

/*
 * @function	get_audio_buffer_ptr
 * @abstract	Allows access to the central Input & Output buffers
 * @param		name		desired buffer's name according to the BUFFER_NAME_t enum
 * @result		returns the buffer's pointer
 */
float* get_audio_buffer_ptr(BUFFER_NAME_t name);

#endif /* AUDIO_PROCESSING_H */

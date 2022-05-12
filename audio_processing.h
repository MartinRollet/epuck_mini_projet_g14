#ifndef AUDIO_PROCESSING_H
#define AUDIO_PROCESSING_H

#define SAMPLE_SIZE	4096

typedef enum {FRONT_INPUT, FRONT_OUTPUT} BUFFER_NAME_t;


void processAudioData(int16_t *data, uint16_t num_samples);

void listen(void);

float* get_audio_buffer_ptr(BUFFER_NAME_t name);

#endif /* AUDIO_PROCESSING_H */

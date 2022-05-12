#ifndef CLASSIFIER_H
#define CLASSIFIER_H

typedef enum {CMD_NOISE, CMD_GO, CMD_RIGHT, CMD_LEFT, CMD_BACK} Command;


void classifier_init(void);

void classifier_predict(float *data, Command *cmd);

#endif /* CLASSIFIER_H */

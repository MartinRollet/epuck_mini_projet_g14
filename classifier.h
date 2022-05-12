#ifndef CLASSIFIER_H
#define CLASSIFIER_H

typedef enum {N, G, L, R, B} Command;


void classifier_init(void);

void classifier_predict(float *data, Command *cmd);

#endif /* CLASSIFIER_H */

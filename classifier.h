#ifndef CLASSIFIER_H
#define CLASSIFIER_H


typedef enum {LISTEN, GO, LEFT, RIGHT, BACK} State;

void classifier_init(void);

void classifier_predict(float *data);

#endif /* CLASSIFIER_H */

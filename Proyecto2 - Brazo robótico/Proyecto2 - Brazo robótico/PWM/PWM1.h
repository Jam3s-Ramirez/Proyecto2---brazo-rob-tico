/*
 * PWM1.h
 *
 * Created: 3/05/2024 18:37:03
 *  Author: James Ramirez
 */ 

#ifndef PWM1_H_
#define PWM1_H_
#include <avr/io.h>
#include <stdint.h>

void initPWM1(void);
void writecanalA(float valorADC);
void writecanalB(float valorADC);
float map1(float x, float in_min, float in_max, float out_min, float out_max);





#endif /* PWM1_H_ */
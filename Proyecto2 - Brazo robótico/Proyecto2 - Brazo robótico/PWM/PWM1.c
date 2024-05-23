/*
 * PWM1.c
 *
 * Created: 3/05/2024 18:37:30
 *  Author: James Ramirez
 */ 

#include "PWM1.h"

void initPWM1(void){
	DDRB |= (1<<DDB1);
	DDRB |= (1<<DDB2);	//CONFIGURATION OUTPUT PB1 AND PB2
	TCCR1A = (1<<COM1A1)|(1<<COM1B1)|(0<<COM1A0);
	TCCR1A |= (1<<WGM11)|(0<<WGM10);
	TCCR1B = (1<<WGM13)|(1<<WGM12);
	TCCR1B |= (0<<CS12)|(1<<CS11)|(0<<CS10);	// Prescaler de 8
	ICR1 = 39999;	// VALOR TOP DE ICR1
}

void writecanalA(float adcV){
	OCR1A = map1(adcV, 0, 1024, 1000, 4800);
}

void writecanalB(float adcV){
	OCR1B = map1(adcV, 0, 1024, 1000, 4800);
}

float map1(float x, float in_min, float in_max, float out_min, float out_max){
	return((x - in_min)*(out_max - out_min)/(in_max - in_min)) + out_min;
}
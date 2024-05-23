/*
 * ADC.c
 *
 * Created: 3/05/2024 18:33:26
 *  Author: James Ramirez
 */ 

#include "ADC.h"

void initADC(void){
	ADMUX |= (1<<REFS0);	// VREF = AVCC
	ADMUX &= ~(1<<REFS1);	// VREF = AVCC
	ADMUX &= ~(1<<ADLAR);	// 10bits
	
	// Prescaler 128 - 16M/128 = 125kHz
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	
	ADCSRA |= (1<<ADEN);	// ON ADC
	
	// DESABILITAR ENTRADAS DIGITALES DE PC0, PC1 Y PC2.
	DIDR0 |= (1<<ADC0D)|(1<<ADC1D)|(1<<ADC2D);
}

uint16_t adcread(uint8_t canal){
	ADMUX = (ADMUX & 0xF0)|canal;	// SELECCIONA EL CANAL A USAR
	ADCSRA |= (1<<ADSC);	// INICIA LA CONVERSION DEL ADC
	while ((ADCSRA)&(1<<ADSC));	// BUCLE DE QUE HASTA QUE FINALICE LA CONVERSION
	return(ADC);
}

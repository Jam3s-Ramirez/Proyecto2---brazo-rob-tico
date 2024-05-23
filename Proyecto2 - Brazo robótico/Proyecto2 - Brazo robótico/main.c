/**************************
Universidad del Valle de Guatemala
Programación de Microcrontroladores
Proyecto: Proyecto 2				
Hardware: ATMEGA328p
Created: 26/04/2024 17:02:04
Author : James Ramírez
***************************/

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "PWM/PWM0.h"
#include "PWM/PWM1.h"
#include "PWM/PWM2.h"
#include "ADC/ADC.h"
#include <avr/eeprom.h>

#define EEPROM_ADDRESS_ADC_PC0_PC3 0x00

void initADC();
float adcvalue1 = 0;
float adcvalue2 = 0;
float adcvalue3 = 0;
float adcvalue4 = 0;
uint8_t estado = 0;

// Función para leer un valor de ADC de un pin específico
uint8_t readADC(uint8_t pin) {
	// Mapear el pin a la correspondiente entrada del ADC
	uint8_t canal = 0;	
	switch (pin) {
		case 0: canal = 0; break; // PC0
		case 1: canal = 1; break; // PC1
		case 2: canal = 2; break; // PC2
		case 3: canal = 3; break; // PC3
	}
	// Realizar la conversión ADC y devolver el resultado
	return adcread(canal);
}

// Función para escribir los valores del ADC en la EEPROM
void escribirADCEnEEPROM(uint8_t offset) {
	eeprom_update_byte((uint8_t *)(EEPROM_ADDRESS_ADC_PC0_PC3 + offset), readADC(0));
	eeprom_update_byte((uint8_t *)(EEPROM_ADDRESS_ADC_PC0_PC3 + offset + 1), readADC(1));
	eeprom_update_byte((uint8_t *)(EEPROM_ADDRESS_ADC_PC0_PC3 + offset + 2), readADC(2));
	eeprom_update_byte((uint8_t *)(EEPROM_ADDRESS_ADC_PC0_PC3 + offset + 3), readADC(3));
}

// Función para leer los valores de la EEPROM y actualizar los valores ADC
void leerEEPROMYActualizarADC(uint8_t offset) {
	adcvalue1 = eeprom_read_byte((uint8_t *)(EEPROM_ADDRESS_ADC_PC0_PC3 + offset));
	adcvalue2 = eeprom_read_byte((uint8_t *)(EEPROM_ADDRESS_ADC_PC0_PC3 + offset + 1));
	adcvalue3 = eeprom_read_byte((uint8_t *)(EEPROM_ADDRESS_ADC_PC0_PC3 + offset + 2));
	adcvalue4 = eeprom_read_byte((uint8_t *)(EEPROM_ADDRESS_ADC_PC0_PC3 + offset + 3));
}

int main() {
	cli();
	
	initADC();
	initPWM1();
	initPWM0A(no_invertido, 1024);
	initPWM2A(no_invertido, 1024);
	
	// Habilitar las interrupciones por cambio de pin
	
	PCICR |= (1<<PCIE0);
	PCMSK0 |= (1<<PCINT0);
	
	PCICR |= (1<<PCIE2);
	PCMSK2 |= (1<<PCINT2) | (1<<PCINT3) | (1<<PCINT4) | (1<<PCINT7);
	
	// Configurar los botones como entrada
	DDRD &= ~(1<<DDD7) & ~(1<<DDD4) & ~(1<<DDD3) & ~(1<<DDD2); // Pin PD4, PD3, PD2 como entrada
	PORTD |= (1<<PORTD7) | (1<<PORTD4) | (1<<PORTD3) | (1<<PORTD2); // Habilita Pull-Up
	
	DDRB &= ~(1<<DDB0); // Pin PB0 como entrada
	PORTB |= (1<<PORTB0); // Habilita Pull-Up
	
	// Configurar los LEDs como salida
	DDRB |= (1 << DDB5); // Pin PB5 como salida (LED)
	DDRD |= (1 << DDD7); // Pin PB5 como salida (LED)
	
	sei(); // Habilitar interrupciones globales
	
	// Limpiar la bandera de interrupción PCINT0
	PCIFR |= (1<<PCIF0);
	
	while (1) {
		
		if (estado == 0) {
			PORTB |= (1 << PB5); // Encender LED
			PORTD &= ~(1 << PD7); // Apagar LED (PD7 si tienes otro LED aquí)
			
			adcvalue1 = readADC(0);
			writecanalA(adcvalue1);
			_delay_ms(10);
			
			adcvalue2 = readADC(1);
			writecanalB(adcvalue2);
			_delay_ms(10);
			
			adcvalue3 = readADC(2);
			updateDutyCA2(adcvalue3);
			_delay_ms(10);
			
			adcvalue4 = readADC(3);
			updateDutyCA(adcvalue4);
			_delay_ms(10);
		}
		if (estado == 1) {
			PORTB &= ~(1 << PB5); // Apagar LED
			PORTD |= (1 << PD7); // Encender LED (PD7 si tienes otro LED aquí)
		}
	}
}

// Interrupción para los pulsadores
ISR(PCINT0_vect) {
	if (!(PINB & (1 << PB0))) {
		_delay_ms(5); // Antirrebote
		if (!(PINB & (1 << PB0))) {
			estado++;
			if (estado >= 2) {
				estado = 0; // Reiniciar la variable estado a 0
			}
		}
	}
}

ISR(PCINT2_vect) {
	if (estado == 0) {
		if (!(PIND & (1 << PD2))) {
			_delay_ms(5); // Antirrebote
			if (!(PIND & (1 << PD2))) {
				escribirADCEnEEPROM(0);
			}
		}
		if (!(PIND & (1 << PD3))) {
			_delay_ms(5); // Antirrebote
			if (!(PIND & (1 << PD3))) {
				escribirADCEnEEPROM(4);
			}
		}
		if (!(PIND & (1 << PD4))) {
			_delay_ms(5); // Antirrebote
			if (!(PIND & (1 << PD4))) {
				escribirADCEnEEPROM(8);
			}
		}
		if (!(PIND & (1 << PD7))) {
			_delay_ms(5); // Antirrebote
			if (!(PIND & (1 << PD7))) {
				escribirADCEnEEPROM(12);
			}
		}
	}
	if (estado == 1) {
		if (!(PIND & (1 << PD2))) {
			_delay_ms(5); // Antirrebote
			if (!(PIND & (1 << PD2))) {
				leerEEPROMYActualizarADC(0);
			}
		}
		if (!(PIND & (1 << PD3))) {
			_delay_ms(5); // Antirrebote
			if (!(PIND & (1 << PD3))) {
				leerEEPROMYActualizarADC(4);
			}
		}
		if (!(PIND & (1 << PD4))) {
			_delay_ms(5); // Antirrebote
			if (!(PIND & (1 << PD4))) {
				leerEEPROMYActualizarADC(8);
			}
		}
		if (!(PIND & (1 << PD7))) {
			_delay_ms(5); // Antirrebote
			if (!(PIND & (1 << PD7))) {
				leerEEPROMYActualizarADC(12);
			}
		}
	}
}






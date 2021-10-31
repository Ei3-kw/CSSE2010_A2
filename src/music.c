#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <avr/io.h>
#define F_CPU 8000000UL	// 8MHz
#include <stdint.h>

static const uint16_t freq_time[56][3]{
	{1, 185, 59}, {0, 185, 178}, {1, 165, 59}, {0, 165, 178}, {1, 165, 59}, {0, 165, 178}, {1, 165, 297}, 
	{0, 165, 178}, {1, 165, 59}, {0, 165, 178}, {1, 165, 59}, {0, 165, 178}, {1, 165, 59}, {0, 165, 178}, 
	{1, 156, 59}, {0, 156, 178}, {1, 156, 59}, {0, 156, 178}, {1, 165, 59}, {0, 165, 178}, {1, 165, 59}, 
	{0, 165, 178}, {1, 277, 297}, {0, 277, 178}, {1, 247, 59}, {0, 247, 178}, {1, 208, 59}, {0, 208, 178}, 
	{1, 185, 59}, {0, 185, 178}, {1, 165, 59}, {0, 165, 178}, {1, 165, 59}, {0, 165, 178}, {1, 165, 297}, 
	{0, 165, 178}, {1, 165, 59}, {0, 165, 178}, {1, 165, 59}, {0, 165, 178}, {1, 165, 59}, {0, 165, 178}, 
	{1, 139, 59}, {0, 139, 178}, {1, 139, 59}, {0, 139, 178}, {1, 123, 59}, {0, 123, 178}, {1, 123, 59}, 
	{0, 123, 178}, {1, 277, 297}, {0, 277, 178}, {1, 247, 59}, {0, 247, 178}, {1, 208, 59}, {0, 208, 178}
};

uint16_t freq_to_clock_period(uint16_t freq) {
	return (1000000UL / freq);	// UL makes the constant an unsigned long (32 bits)
								// and ensures we do 32 bit arithmetic, not 16
}

// Return the width of a pulse (in clock cycles) given a duty cycle (%) and
// the period of the clock (measured in clock cycles)
uint16_t duty_cycle_to_pulse_width(float dutycycle, uint16_t clockperiod) {
	return (dutycycle * clockperiod) / 100;
}

int main() {
	uint16_t freq = 0;	// Hz
	float dutycycle = 2;	// %
	uint16_t clockperiod = freq_to_clock_period(freq);
	uint16_t pulsewidth = duty_cycle_to_pulse_width(dutycycle, clockperiod);
	
	// Make pin OC1B be an output
	DDRD = (1 << 4);
	
	// Set the maximum count value for timer/counter 1 to be one less than the clockperiod
	OCR1A = clockperiod - 1;
	
	// Set the count compare value based on the pulse width. The value will be 1 less
	// than the pulse width - unless the pulse width is 0.
	OCR1B = 128;
	
	// Set up timer/counter 1 for Fast PWM, counting from 0 to the value in OCR1A
	// before reseting to 0. Count at 1MHz (CLK/8).
	// Configure output OC1B to be clear on compare match and set on timer/counter
	// overflow (non-inverting mode).
	TCCR1A = (1<<COM1B1) | (1<<WGM11) | (1<<WGM10);
	TCCR1B = (1<<WGM13) | (1<<WGM12) | (1<<CS11);
}
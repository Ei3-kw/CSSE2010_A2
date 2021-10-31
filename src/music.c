#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <avr/io.h>
#define F_CPU 8000000L	// 8MHz
#include <stdint.h>

#include "music.h"

#define NUM_NOTES 56
static const uint16_t time_freq[56][3] = {
	{1, 59, 197}, {0, 178, 197}, {1, 59, 177}, {0, 178, 177}, {1, 59, 177}, {0, 178, 177}, {1, 297, 177}, 
	{0, 178, 177}, {1, 59, 177}, {0, 178, 177}, {1, 59, 177}, {0, 178, 177}, {1, 59, 177}, {0, 178, 177}, 
	{1, 59, 168}, {0, 178, 168}, {1, 59, 168}, {0, 178, 168}, {1, 59, 177}, {0, 178, 177}, {1, 59, 177}, 
	{0, 178, 177}, {1, 297, 289}, {0, 178, 289}, {1, 59, 259}, {0, 178, 259}, {1, 59, 220}, {0, 178, 220}, 
	{1, 59, 197}, {0, 178, 197}, {1, 59, 177}, {0, 178, 177}, {1, 59, 177}, {0, 178, 177}, {1, 297, 177}, 
	{0, 178, 177}, {1, 59, 177}, {0, 178, 177}, {1, 59, 177}, {0, 178, 177}, {1, 59, 177}, {0, 178, 177}, 
	{1, 59, 151}, {0, 178, 151}, {1, 59, 151}, {0, 178, 151}, {1, 59, 135}, {0, 178, 135}, {1, 59, 135}, 
	{0, 178, 135}, {1, 297, 289}, {0, 178, 289}, {1, 59, 259}, {0, 178, 259}, {1, 59, 220}, {0, 178, 220}
};
// static const uint16_t time_freq[NUM_NOTES][3] = {
// 	{1, 59, 185}, {0, 178, 185}, {1, 59, 165}, {0, 178, 165}, {1, 59, 165}, {0, 178, 165}, {1, 297, 165}, 
// 	{0, 178, 165}, {1, 59, 165}, {0, 178, 165}, {1, 59, 165}, {0, 178, 165}, {1, 59, 165}, {0, 178, 165}, 
// 	{1, 59, 156}, {0, 178, 156}, {1, 59, 156}, {0, 178, 156}, {1, 59, 165}, {0, 178, 165}, {1, 59, 165}, 
// 	{0, 178, 165}, {1, 297, 277}, {0, 178, 277}, {1, 59, 247}, {0, 178, 247}, {1, 59, 208}, {0, 178, 208}, 
// 	{1, 59, 185}, {0, 178, 185}, {1, 59, 165}, {0, 178, 165}, {1, 59, 165}, {0, 178, 165}, {1, 297, 165}, 
// 	{0, 178, 165}, {1, 59, 165}, {0, 178, 165}, {1, 59, 165}, {0, 178, 165}, {1, 59, 165}, {0, 178, 165}, 
// 	{1, 59, 139}, {0, 178, 139}, {1, 59, 139}, {0, 178, 139}, {1, 59, 123}, {0, 178, 123}, {1, 59, 123}, 
// 	{0, 178, 123}, {1, 297, 277}, {0, 178, 277}, {1, 59, 247}, {0, 178, 247}, {1, 59, 208}, {0, 178, 208}
// };

uint16_t event;
uint16_t duration;
uint8_t i = 0;
bool mutee = 0;

uint16_t freq_to_clock_period(uint16_t freq) {
	return (1000000UL / freq);	// UL makes the constant an unsigned long (32 bits)
								// and ensures we do 32 bit arithmetic, not 16
}

// Return the width of a pulse (in clock cycles) given a duty cycle (%) and
// the period of the clock (measured in clock cycles)
uint16_t duty_cycle_to_pulse_width(float dutycycle, uint16_t clockperiod) {
	return (dutycycle * clockperiod) / 100;
}

void bgm(void) {
	
	DDRD = (1 << 4);
	
	TCCR1A = (1<<COM1B1) | (1<<WGM11) | (1<<WGM10);
	TCCR1B = (1<<WGM13) | (1<<WGM12) | (1<<CS11);

	event = time_freq[i][0];
	duration = time_freq[i][1];
	OCR1A = freq_to_clock_period(time_freq[i][2]) - 1;
	OCR1B = 64;	
}

void note_count_down(void) {
	duration -= 1;

	if (duration > 0) {
		if (!event) {
			DDRD = 0;
		}
	} else {
		i += 1;
		if (i == 56) {
			i = 0;
		}
	}
}

void bomb_count_down(bool on, uint8_t x) {
	DDRD = (1 << 4);
	
	TCCR1A = (1<<COM1B1) | (1<<WGM11) | (1<<WGM10);
	TCCR1B = (1<<WGM13) | (1<<WGM12) | (1<<CS11);

	OCR1A = 440 + x;
	OCR1B = 64;

	if (!on || mutee) {
		DDRD = 0;
	}
}

void mute(void) {
	mutee = !mutee;
}
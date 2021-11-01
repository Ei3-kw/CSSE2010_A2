#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <avr/io.h>
#define F_CPU 8000000L	// 8MHz
#include <stdint.h>

#include "music.h"
#include "game.h"
#include "timer0.h"

// I WANNA PLAY AVICII LEVELS (IT'S 10 Y/O NOW)
// 
// #define NUM_NOTES 56
// static const uint16_t time_freq[NUM_NOTES][3] = {
// 	{1, 59, 197}, {0, 178, 197}, {1, 59, 177}, {0, 178, 177}, {1, 59, 177}, {0, 178, 177}, {1, 297, 177}, 
// 	{0, 178, 177}, {1, 59, 177}, {0, 178, 177}, {1, 59, 177}, {0, 178, 177}, {1, 59, 177}, {0, 178, 177}, 
// 	{1, 59, 168}, {0, 178, 168}, {1, 59, 168}, {0, 178, 168}, {1, 59, 177}, {0, 178, 177}, {1, 59, 177}, 
// 	{0, 178, 177}, {1, 297, 289}, {0, 178, 289}, {1, 59, 259}, {0, 178, 259}, {1, 59, 220}, {0, 178, 220}, 
// 	{1, 59, 197}, {0, 178, 197}, {1, 59, 177}, {0, 178, 177}, {1, 59, 177}, {0, 178, 177}, {1, 297, 177}, 
// 	{0, 178, 177}, {1, 59, 177}, {0, 178, 177}, {1, 59, 177}, {0, 178, 177}, {1, 59, 177}, {0, 178, 177}, 
// 	{1, 59, 151}, {0, 178, 151}, {1, 59, 151}, {0, 178, 151}, {1, 59, 135}, {0, 178, 135}, {1, 59, 135}, 
// 	{0, 178, 135}, {1, 297, 289}, {0, 178, 289}, {1, 59, 259}, {0, 178, 259}, {1, 59, 220}, {0, 178, 220}
// };

bool muted = 0;

uint16_t freq_to_clock_period(uint16_t freq) {
	return (1000000UL / freq);	// UL makes the constant an unsigned long (32 bits)
								// and ensures we do 32 bit arithmetic, not 16
}

// Return the width of a pulse (in clock cycles) given a duty cycle (%) and
// the period of the clock (measured in clock cycles)
uint16_t duty_cycle_to_pulse_width(float dutycycle, uint16_t clockperiod) {
	return (dutycycle * clockperiod) / 100;
}

// void bgm(void) {
	
// 	if (!muted && !game_paused()) {
// 		DDRD = (1 << 4);
		
// 		TCCR1A = (1<<COM1B1) | (1<<WGM11) | (1<<WGM10);
// 		TCCR1B = (1<<WGM13) | (1<<WGM12) | (1<<CS11);
	
// 		OCR1B = 64;	
// 		OCR1A = freq_to_clock_period(time_freq[0][2]) - 1;
	
// 		uint32_t last_note_time = get_current_time();
// 		uint16_t sum_time = 0;
	
// 		for (int i = 0; i < NUM_NOTES; i++) {
// 			sum_time += time_freq[i][1];
// 			while (last_note_time + sum_time > get_current_time()) {
// 				;
// 			}
// 			OCR1A = freq_to_clock_period(time_freq[i+1][2]) - 1;
// 		}
// 	} else {
// 		DDRD = 0;
// 	}
// }

void bomb_count_down(bool on, uint8_t x) {
	DDRD = (1 << 4);
	
	TCCR1A = (1<<COM1B1) | (1<<WGM11) | (1<<WGM10);
	TCCR1B = (1<<WGM13) | (1<<WGM12) | (1<<CS11);

	OCR1A = 440 + x;
	OCR1B = 64;

	if (!on || muted) {
		DDRD = 0;
		OCR1B = 0;
		TCCR1A = 0;
		TCCR1B = 0;
	}
}

void successful_inspection(uint8_t stage) {
	DDRD = (1 << 4);

	if (!muted){
		if (stage == 1) {
			OCR1A = 700;
		} else if (stage == 2) {
			OCR1A = 1000;
		} else if (stage == 3) {
			DDRD = 0;
			OCR1B = 0;
			TCCR1A = 0;
			TCCR1B = 0;
		}
	} else {
		DDRD = 0;
		OCR1B = 0;
		TCCR1A = 0;
		TCCR1B = 0;
	}

	TCCR1A = (1<<COM1B1) | (1<<WGM11) | (1<<WGM10);
	TCCR1B = (1<<WGM13) | (1<<WGM12) | (1<<CS11);

	OCR1B = 64;
}

void bomb_explode(uint8_t stage) {
	DDRD = (1 << 4);

	if (!muted) {
		if (stage == 1) {
			OCR1A = 17600;
			OCR1B = 64;
		} else if (stage == 2) {
			OCR1A = 35200;
			OCR1B = 32;
		}
	} else {
		DDRD = 0;
		OCR1B = 0;
		TCCR1A = 0;
		TCCR1B = 0;
	}
	
	TCCR1A = (1<<COM1B1) | (1<<WGM11) | (1<<WGM10);
	TCCR1B = (1<<WGM13) | (1<<WGM12) | (1<<CS11);

}

void mute(void) {
	muted = !muted;
}
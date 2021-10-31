#ifndef MUSIC_H_
#define MUSIC_H_

#include <inttypes.h>
#include <stdbool.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <avr/io.h>
#define F_CPU 8000000L	// 8MHz
#include <stdint.h>

void bgm(void);

void note_count_down(void);

void bomb_count_down(bool on, uint8_t x);

void mute(void);

#endif
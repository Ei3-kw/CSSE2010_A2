/*
 * project.c
 *
 * Main file for IN students
 *
 * Authors: Peter Sutton, Luke Kamols
 * Diamond Miners Inspiration: Daniel Cumming
 * Modified by Jiayi WANG
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdbool.h>

#include "game.h"
#include "display.h"
#include "ledmatrix.h"
#include "buttons.h"
#include "serialio.h"
#include "terminalio.h"
#include "timer0.h"
#include "music.h"

#define F_CPU 8000000L
#include <util/delay.h>

// Function prototypes - these are defined below (after main()) in the order
// given here
void initialise_hardware(void);
void start_screen(void);
void new_game(void);
void play_game(void);
void handle_game_over(void);

/////////////////////////////// main //////////////////////////////////
int main(void) {
	// Setup hardware and call backs. This will turn on 
	// interrupts.
	initialise_hardware();
	
	// Show the splash screen message. Returns when display is complete
	start_screen();
	
	// Loop forever,
	while(1) {
		new_game();
		play_game();
		handle_game_over();
	}
}

void initialise_hardware(void) {
	ledmatrix_setup();
	init_button_interrupts();
	// Setup serial port for 19200 baud communication with no echo
	// of incoming characters
	init_serial_stdio(19200,0);
	
	init_timer0();
	
	DDRC |= (1 << 7);

	// Turn on global interrupts
	sei();
}

void start_screen(void) {
	// Clear terminal screen and output a message
	clear_terminal();
	move_terminal_cursor(10,10);
	printf_P(PSTR("Diamond Miners"));
	move_terminal_cursor(10,12);
	printf_P(PSTR("CSSE2010/7201 project by <Jiayi WANG>"));
	move_terminal_cursor(10,14);
	printf_P(PSTR("Student num: 46822394"));
	
	// Output the static start screen and wait for a push button 
	// to be pushed or a serial input of 's'
	start_display();
	
	// Wait until a button is pressed, or 's' is pressed on the terminal
	while(1) {
		// First check for if a 's' is pressed
		// There are two steps to this
		// 1) collect any serial input (if available)
		// 2) check if the input is equal to the character 's'
		char serial_input = -1;
		if (serial_input_available()) {
			serial_input = fgetc(stdin);
		}
		// If the serial input is 's', then exit the start screen
		if (serial_input == 's' || serial_input == 'S') {
			break;
		}
		// Next check for any button presses
		int8_t btn = button_pushed();
		if (btn != NO_BUTTON_PUSHED) {
			break;
		}
	}
}

void new_game(void) {
	// Clear the serial terminal
	clear_terminal();
	// Initialise the game and display
	initialise_game();
	
	// Clear a button push or serial input if any are waiting
	// (The cast to void means the return value is ignored.)
	(void)button_pushed();
	clear_serial_input_buffer();
}

void play_game(void) {
	
	uint32_t last_flash_time, current_time, last_flash, 
		inspecting_time;
	uint8_t btn; //the button pushed
	bool inspected = 0;
	
	last_flash = last_flash_time = get_current_time();
	inspecting_time = UINT32_MAX;

	// FAILED RIPPP
	// bgm();
	
	move_terminal_cursor(10,10);
	printf_P(PSTR("Cheat off"));
	move_terminal_cursor(10,12);
	printf_P(PSTR("Diamonds: 0"));

	// We play the game until it's over
	while(!is_game_over()) {

		while (!game_paused() && !is_game_over()) {
			// We need to check if any button has been pushed, this will be
			// NO_BUTTON_PUSHED if no button has been pushed
			btn = button_pushed();

			char serial_input = -1;

			if (serial_input_available()) {
				serial_input = fgetc(stdin);
			}
			
			if (btn == BUTTON0_PUSHED || serial_input == 'd' || serial_input == 'D') {
				// If button 0 is pushed, or 'd' is pressed, move right, 
				// i.e increase x by 1 and leave y the same
				move_player(1, 0);
			} else if (btn == BUTTON1_PUSHED || serial_input == 'a' || serial_input == 'A') {
				// If button 1 is pushed, or 'a' is pressed, move left, 
				// i.e decrease x by 1 and leave y the same
				move_player(-1, 0);
			} else if (btn == BUTTON2_PUSHED || serial_input == 'w' || serial_input == 'W') {
				// If button 2 is pushed, or 'w' is pressed, move up, 
				// i.e increase y by 1 and leave x the same
				move_player(0, 1);
			} else if (btn == BUTTON3_PUSHED || serial_input == 's' || serial_input == 'S') {
				// If button 3 is pushed, or 's' is pressed, move right, 
				// i.e decrease y by 1 and leave x the same
				move_player(0, -1);
			}

			if (serial_input == 'e' || serial_input == 'E') {
				inspecting();
				inspected = 1;
				inspecting_time = get_current_time();
			}

			if (serial_input == 'c' || serial_input == 'C') {
				cheat_mode();
			}

			if (serial_input == ' ') {
				bombing();
			}

			if (serial_input == 'p' || serial_input == 'P') {
				pause();
			}

			if (serial_input == 'f' || serial_input == 'F') {
				if (vision()) {
					hide();
					wanda(1);
				} else {
					wanda(0);
					seek();
				}
			}

			if (serial_input == 'm' || serial_input == 'M') {
				mute();
			}

			current_time = get_current_time();
			if (current_time >= last_flash_time + 500) {
				// 500ms (0.5 second) has passed since the last time we
				// flashed the cursor, so flash the cursor
				flash_facing();
				
				// Update the most recent time the cursor was flashed
				last_flash_time = current_time;
			}

			if ((calculate_distance() == 4 
				&& current_time >= last_flash + 1500) 
				|| (calculate_distance() == 3 
					&& current_time >= last_flash + 1000) 
				|| (calculate_distance() == 2 
					&& current_time >= last_flash + 500) 
				|| (calculate_distance() == 1 
					&& current_time >= last_flash + 250)) {
				flashing();
				last_flash = current_time;
			}

			if (get_current_time() >= inspecting_time + 363 && inspected 
				&& get_current_time() < inspecting_time + 726) {
				successful_inspection(2);
			} else if (get_current_time() >= inspecting_time + 726 && inspected) {
				successful_inspection(3);
				inspected = 0;
			}

		}

		char serial_input = -1;
		if (serial_input_available()) {
			serial_input = fgetc(stdin);
		}

		if (serial_input == 'p' || serial_input == 'P') {
			pause();
		}
	}
	// We get here if the game is over.
}

void handle_game_over() {
	if (!has_won()) {
		move_terminal_cursor(10,14);
		printf_P(PSTR("GAME OVER"));
		move_terminal_cursor(10,15);
		printf_P(PSTR("Press a button to start again"));
		
		while(button_pushed() == NO_BUTTON_PUSHED) {
			; // wait
		}
	}
	restart();
}

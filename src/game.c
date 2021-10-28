/*
 * game.c
 *
 * Contains functions relating to the play of Diamond Miners
 *
 * Author: Luke Kamols
 */ 

#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "game.h"
#include "display.h"
#include "terminalio.h"
#include "timer0.h"

#define PLAYER_START_X  0;
#define PLAYER_START_Y  0;
#define FACING_START_X  1;
#define FACING_START_Y  0;

// the initial game layout
// the values 0, 3, 4 and 5 are defined in display.h
// note that this is not laid out in such a way that starting_layout[x][y]
// does not correspond to an (x,y) coordinate but is a better visual
// representation
static const uint8_t starting_layout[HEIGHT][WIDTH] = 
		{
			{0, 3, 0, 3, 0, 0, 0, 4, 4, 0, 0, 4, 0, 4, 0, 4},
			{0, 4, 0, 4, 0, 0, 0, 3, 4, 4, 3, 4, 0, 3, 0, 4},
			{0, 4, 0, 4, 4, 4, 4, 0, 3, 0, 0, 0, 0, 4, 0, 4},
			{5, 4, 0, 4, 0, 0, 3, 0, 0, 4, 0, 0, 0, 4, 0, 0},
			{4, 4, 3, 4, 5, 0, 4, 0, 0, 4, 3, 4, 0, 0, 4, 4},
			{0, 0, 0, 4, 4, 4, 4, 0, 4, 0, 0, 0, 4, 3, 0, 4},
			{0, 0, 0, 3, 0, 0, 3, 0, 3, 0, 3, 0, 3, 0, 0, 4},
			{0, 0, 0, 4, 0, 0, 3, 0, 4, 0, 0, 3, 3, 0, 5, 4} 
		};

static const uint8_t next_lv_layout[HEIGHT][WIDTH] = 
		{
			{4, 4, 0, 5, 0, 4, 0, 0, 4, 0, 4, 4, 0, 3, 4, 4},
			{3, 4, 0, 4, 4, 4, 4, 4, 3, 0, 0, 4, 0, 0, 4, 4},
			{0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 4},
			{4, 4, 0, 4, 0, 4, 3, 4, 4, 4, 4, 4, 5, 0, 3, 5},
			{3, 3, 3, 4, 4, 4, 5, 4, 5, 4, 0, 0, 0, 3, 3, 4},
			{0, 0, 0, 4, 0, 4, 3, 0, 0, 3, 0, 3, 0, 4, 0, 4},
			{0, 0, 0, 3, 0, 4, 4, 0, 4, 3, 4, 0, 4, 0, 0, 4},
			{0, 0, 0, 3, 0, 0, 4, 4, 0, 3, 5, 4, 0, 4, 4, 4}
		};
		
#define NUM_DIRECTIONS 4
static const uint8_t directions[NUM_DIRECTIONS][2] = { {0,1}, {0,-1}, {1,0}, {-1,0}};

#define NUM_VISIONS 20
static const int8_t wanda_vision[NUM_VISIONS][2] = {{-2, -1}, {-2, 0}, {-2, 1}, {-1, -2}, 
	{-1, -1}, {-1, 0}, {-1, 1}, {-1, 2}, {0, -2}, {0, -1}, {0, 1}, {0, 2}, {1, -2}, 
	{1, -1}, {1, 0}, {1, 1}, {1, 2}, {2, -1}, {2, 0}, {2, 1}};

// variables for the current state of the game
uint8_t playing_field[WIDTH][HEIGHT]; // what is currently located at each square
uint8_t visible[WIDTH][HEIGHT]; // whether each square is currently visible
uint8_t player_x;
uint8_t player_y;
uint8_t facing_x;
uint8_t facing_y;
uint8_t facing_visible;
bool cheat_on = 0;
uint8_t diamond_collected = 0;
uint8_t distance;
uint8_t flashing_visible = 0;
bool bomb_visible = 0;
uint16_t bomb_placed_time = UINT16_MAX;
uint16_t last_flash_time = 2000;
uint16_t exploding_time = UINT16_MAX;
uint8_t bomb_x = UINT8_MAX;
uint8_t bomb_y = UINT8_MAX;
bool game_over = 0;
bool paused = 0;
bool next_lv = 0;
bool won = 0;
bool vision_bubble = 0;

// function prototypes for this file
void discoverable_dfs(uint8_t x, uint8_t y);
void initialise_game_display(void);
void initialise_game_state(void);

/*
 * initialise the game state, sets up the playing field, visibility
 * the player and the player direction indicator
 */
void initialise_game_state(void) {
	// initialise the player position and the facing position
	player_x = PLAYER_START_X;
	player_y = PLAYER_START_Y;
	facing_x = FACING_START_X;
	facing_y = FACING_START_Y;
	facing_visible = 1;
	
	// go through and initialise the state of the playing_field
	for (int x = 0; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			// initialise this square based on the starting layout
			// the indices here are to ensure the starting layout
			// could be easily visualised when declared
			if (next_lv) {
				playing_field[x][y] = next_lv_layout[HEIGHT - 1 - y][x];
			} else {
				playing_field[x][y] = starting_layout[HEIGHT - 1 - y][x];
			}
			// set all squares to start not visible, this will be
			// updated once the display is initialised as well
			visible[x][y] = 0;
		}
	}	
}

/*
 * initialise the display of the game, shows the player and the player
 * direction indicator. 
 * executes a visibility search from the player's starting location
 */
void initialise_game_display(void) {
	// initialise the display
	initialise_display();
	// make the entire playing field undiscovered to start
	for (int x = 0; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
			update_square_colour(x, y, UNDISCOVERED);
		}
	}
	// now explore visibility from the starting location
	discoverable_dfs(player_x, player_y);
	// make the player and facing square visible
	update_square_colour(player_x, player_y, PLAYER);
	update_square_colour(facing_x, facing_y, FACING);
}

void initialise_game(void) {
	// to initialise the game, we need to initialise the state (variables)
	// and the display
	initialise_game_state();
	initialise_game_display();
}

uint8_t in_bounds(uint8_t x, uint8_t y) {
	// a square is in bounds if 0 <= x < WIDTH && 0 <= y < HEIGHT
	return x >= 0 && x < WIDTH && y >= 0 && y < HEIGHT;
}

uint8_t get_object_at(uint8_t x, uint8_t y) {
	// check the bounds, anything outside the bounds
	// will be considered an unbreakable wall
	if (!in_bounds(x,y)) {
		return UNBREAKABLE;
	} else {
		//if in the bounds, just index into the array
		return playing_field[x][y];
	}
}

void flash_facing(void) {
	// only flash the facing cursor if it is in bounds
	if (in_bounds(facing_x, facing_y)) {
		if (facing_visible) {
			// we need to flash the facing cursor off, it should be replaced by
			// the colour of the piece which is at that location
			uint8_t piece_at_cursor = get_object_at(facing_x, facing_y);
			update_square_colour(facing_x, facing_y, piece_at_cursor);
		
		} else {
			// we need to flash the facing cursor on
			update_square_colour(facing_x, facing_y, FACING);
		}
		facing_visible = !facing_visible; //alternate between 0 and 1
	}
}

// check the header file game.h for a description of what this function should do
// it contains a few extra hints
void move_player(uint8_t dx, uint8_t dy) {
	// suggestions for implementation:
	// 1: remove the display of the player at the current location
	//    (and replace it with whatever else is at that location)
	// 2: determine if a move is possible
	// 3: if the player can move, update the positional knowledge of the player, 
	//    this will include variables player_x and player_y
	// 4: display the player at the new location
	if (get_object_at(player_x + dx, player_y + dy) != BREAKABLE 
		&& get_object_at(player_x + dx, player_y + dy) != UNBREAKABLE
		&& get_object_at(player_x + dx, player_y + dy) != INSPECTED_BREAKABLE) {
	
		playing_field[player_x][player_y] = EMPTY_SQUARE;
		update_square_colour(player_x, player_y, EMPTY_SQUARE);
		
		if (vision_bubble) {
			wanda(0);
		}

		player_x += dx;
		player_y += dy;
		collecting_diamonds(player_x, player_y);
		playing_field[player_x][player_y] = PLAYER;
		update_square_colour(player_x, player_y, PLAYER);
		
		if (calculate_distance() > 4) {
			PORTC &= ~(1 << 7);
		}

		if (bomb_placed_time != UINT16_MAX 
			&& (player_x != bomb_x 
				|| player_y != bomb_y)) {
			update_square_colour(bomb_x, bomb_y, BOMB);
		}

		if (vision_bubble) {
			wanda(1);
		}

	} else {
		if (player_x + dx == WIDTH) {
			check_win();
		} 
	}
	update_square_colour(facing_x, facing_y, get_object_at(facing_x, facing_y));
	facing_x = player_x + dx;
	facing_y = player_y + dy;
	flash_facing();
}

void inspecting(void) {
	if (get_object_at(facing_x, facing_y) == BREAKABLE) {
		playing_field[facing_x][facing_y] = INSPECTED_BREAKABLE;
		update_square_colour(facing_x, facing_y, INSPECTED_BREAKABLE);
	}
	cheating();
}

void cheat_mode(void) {
	clear_terminal();
	cheat_on = !cheat_on;
	if (cheat_on) {
		move_terminal_cursor(10,10);
		printf_P(PSTR("Cheat on"));
	} else {
		move_terminal_cursor(10,10);
		printf_P(PSTR("Cheat off"));
	}
	move_terminal_cursor(10,12);
	printf_P(PSTR("Diamonds: %d"), diamond_collected);
}

void cheating(void) {
	if (cheat_on) {
		if (get_object_at(facing_x, facing_y) == INSPECTED_BREAKABLE) {
			playing_field[facing_x][facing_y] = EMPTY_SQUARE;
			update_square_colour(facing_x, facing_y, EMPTY_SQUARE);
			discoverable_dfs(facing_x, facing_y);
		}
	}
}

void collecting_diamonds(uint8_t x, uint8_t y) {
	if (get_object_at(x, y) == DIAMOND) {
		diamond_collected += 1;
		playing_field[x][y] = EMPTY_SQUARE;
		move_terminal_cursor(10,12);
		printf_P(PSTR("Diamonds: %d"), diamond_collected);
	}
}

uint8_t calculate_distance(void) {
	uint8_t x, y;
	distance = HEIGHT + WIDTH;

	for (x = 0; x < WIDTH; ++x) {
		for (y = 0; y < HEIGHT; ++y) {
			if (get_object_at(x, y) == DIAMOND) {
				if (abs(player_x - x) + abs(player_y - y) < distance) {
					distance = abs(player_x - x) + abs(player_y - y);
				}
			}
		}
	}
	return distance;
}

void flashing(void) {
	if (flashing_visible) {
		PORTC |= (1 << 7);
	} else {
		PORTC &= ~(1 << 7);
	}
	flashing_visible = !flashing_visible;
}

void bombing(void) {
	if (bomb_placed_time == UINT16_MAX) {
		bomb_placed_time = 2000;
		bomb_x = player_x;
		bomb_y = player_y;
	}
}

void count_down(void) {
	bomb_placed_time -= 1;

	if (bomb_placed_time <= last_flash_time - bomb_placed_time/10) {
		flash_bomb();
		last_flash_time = bomb_placed_time;
	}

	PORTC &= ~(1 << 6);

	for (int i = 0; i < NUM_DIRECTIONS; i++) {
		uint8_t x = bomb_x + directions[i][0];
		uint8_t y = bomb_y + directions[i][1];

		if (get_object_at(x, y) == PLAYER) {
			PORTC |= (1 << 6);
		}
	}

	if (bomb_x == player_x && bomb_y == player_y) {
		PORTC |= (1 << 6);
	}

	if (bomb_placed_time <= 300) {
		exploding_effect();
	}

	if (bomb_placed_time == 0) {
		exploding();
	}
}

void flash_bomb(void) {
	if (player_x != bomb_x || player_y != bomb_y) {
		if (bomb_visible) {
			update_square_colour(bomb_x, bomb_y, EMPTY_SQUARE);
		} else {
			update_square_colour(bomb_x, bomb_y, BOMB);
		}
	}
	bomb_visible = !bomb_visible; //alternate between 0 and 1
}


uint16_t get_bomb_placed_time(void) {
	return bomb_placed_time;
}

void exploding(void) {
	bomb_placed_time = UINT16_MAX;

	for (int i = 0; i < NUM_DIRECTIONS; i++) {
		uint8_t x = bomb_x + directions[i][0];
		uint8_t y = bomb_y + directions[i][1];

		if (get_object_at(x, y) == INSPECTED_BREAKABLE 
			|| get_object_at(x, y) == BREAKABLE) {
			playing_field[x][y] = EMPTY_SQUARE;
			update_square_colour(x, y, EMPTY_SQUARE);
			discoverable_dfs(x, y);
		}

		if (get_object_at(x, y) == PLAYER) {
			game_over = 1;
			won = 0;
		}

		if (visible[x][y]) {
			update_square_colour(x, y, get_object_at(x, y));
		} else {
			update_square_colour(x, y, UNDISCOVERED);
		}
	}

	if (bomb_x == player_x && bomb_y == player_y) {
		game_over = 1;
		won = 0;
	} else {
		update_square_colour(bomb_x, bomb_y, EMPTY_SQUARE);
	}

	bomb_x = bomb_y = UINT8_MAX;
	PORTC &= ~(1 << 6);

	if (vision_bubble) {
		hide();
		wanda(1);
	}
}

void exploding_effect(void) {
	// update_square_colour(bomb_x, bomb_y, PLAYER);
	for (int i = 0; i < NUM_DIRECTIONS; i++) {
		uint8_t x = bomb_x + directions[i][0];
		uint8_t y = bomb_y + directions[i][1];
		update_square_colour(x, y, BOMB_AREA);
	}
}

void pause(void) {
	paused = !paused;
}

bool game_paused(void) {
	return paused;
}

bool is_game_over(void) {
	// initially the game never ends
	return game_over;
}

void restart(void) {
	game_over = 0;
	cheat_on = 0;
	diamond_collected = 0;
	won = 0;
	bomb_x = bomb_y = UINT8_MAX;
}

void check_win(void) {
	if (get_object_at(15, 4) == PLAYER) {
		if (next_lv) {
			if (diamond_collected == 6) {
				next_lv = !next_lv;
				game_over = 1;
				won = 1;
			}
		} else {
			if (diamond_collected == 3) {
				next_lv = !next_lv;
				game_over = 1;
				won = 1;
			}
		}
	}
}

bool has_won(void) {
	return won;
}

bool vision(void) {
	vision_bubble = !vision_bubble;
	return vision_bubble;
}

void hide(void) {
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < HEIGHT; j++) {
			update_square_colour(i, j, UNDISCOVERED);
		}
	}
}

void seek(void) {
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < HEIGHT; j++) {
			if (visible[i][j]) {
				update_square_colour(i, j, get_object_at(i, j));
			}
		}
	}
}

void wanda(bool vb) {
	
	for (int i = 0; i < NUM_VISIONS; i++) {
		uint8_t x = player_x + wanda_vision[i][0];
		uint8_t y = player_y + wanda_vision[i][1];
		
		if (in_bounds(x, y)) {
			if (vb) {
				if (x != bomb_x || y != bomb_y) {
					update_square_colour(x, y, get_object_at(x, y));
					update_square_colour(player_x, player_y, PLAYER);
				}
			} else {
				update_square_colour(x, y, UNDISCOVERED);
			}
		}
	}
}

/*
 * given an (x,y) coordinate, perform a depth first search to make any
 * squares reachable from here visible. If a wall is broken at a position
 * (x,y), this function should be called with coordinates (x,y)
 * YOU SHOULD NOT NEED TO MODIFY THIS FUNCTION
 */
void discoverable_dfs(uint8_t x, uint8_t y) {
	uint8_t x_adj, y_adj, object_here;
	// set the current square to be visible and update display
	visible[x][y] = 1;
	object_here = get_object_at(x, y);
	update_square_colour(x, y, object_here);
	// we can continue exploring from this square if it is empty
	if (object_here == EMPTY_SQUARE || object_here == DIAMOND) {
		// consider all 4 adjacent square
		for (int i = 0; i < NUM_DIRECTIONS; i++) {
			x_adj = x + directions[i][0];
			y_adj = y + directions[i][1];
			// if this square is not visible yet, it should be explored
			if (in_bounds(x_adj, y_adj) && !visible[x_adj][y_adj]) {
				// this recursive call implements a depth first search
				// the visible array ensures termination
				discoverable_dfs(x_adj, y_adj);
			}
		}
	}
}
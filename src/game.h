/*
** game.h
**
** Written by Luke Kamols, Daniel Cumming
**
** Function prototypes for those functions available externally
*/

#ifndef GAME_H_
#define GAME_H_

#include <inttypes.h>
#include <stdbool.h>

/*
 * initialise the game, creates the internal game state and updates
 * the display of this game
 */
void initialise_game(void);

/* returns which object is located at position (x,y)
 * the value returned will be EMPTY_SQUARE, BREAKABLE, UNBREAKABLE
 * or DIAMOND
 * if the given coordinates are out of bounds UNBREAKABLE will be returned
 */
uint8_t get_object_at(uint8_t x, uint8_t y);

/*
 * returns 1 if a given (x,y) coordinate is inside the bounds of 
 * the playing field, 0 if it is out of bounds
 */
uint8_t in_bounds(uint8_t x, uint8_t y);

/* update the player direction indicator display, by changing whether
 * it is visible or not, call this function at regular intervals to
 * have the indicator flash
 */
void flash_facing(void);

/*
 * move the position of the player by (dx, dy) if they can be moved
 * the player direction indicator should also be updated by this.
 * the player should move to square (x+dx, y+dy) if there is an
 * EMPTY_SQUARE or DIAMOND at that location.
 * get_object_at(x+dx, y+dy) can be used to check what is at that position
 */
void move_player(uint8_t dx, uint8_t dy);

// returns 1 if the game is over, 0 otherwise
bool is_game_over(void);

void inspecting(void);

void cheat_mode(void);

void cheating(void);

void collecting_diamonds(uint8_t x, uint8_t y);

void cheating(void);

uint8_t calculate_distance(void);

void flashing(void);

void bombing(void);

uint16_t get_bomb_placed_time(void);

void count_down(void);

void flash_bomb(void);

void exploding(void);

void exploding_effect(void);

void pause(void);

bool game_paused(void);

void restart(void);

void check_win(void);

bool has_won(void);

bool vision(void);

void hide(void);

void seek(void);

void wanda(bool vb);

void display_steps(void);

#endif

/*
 * How long? That is the question Robert Cancross posits as his focus returns, 
 * breaking the reverie he had been so graciously lost in. The dappled light 
 * filters through the leaves, falling softly upon his face. The shimmering 
 * warmth reminds him of Tuscany. Tuscany. How long had it been since he was 
 * home? He knew he could not provide an honest answer. And how long till he 
 * would return? ... Would he return? The weight of these questions was heavy 
 * upon him now, and this burden was more than he should like to carry. He 
 * busies himself by picking up equipment - with each item he lifts, the burden 
 * reduces a little more. There's no point waiting any longer, he has come this
 * far and waited this long. One last look towards the hills, before he turns 
 * towards the shaft of the mine, and begins his descent...
 */
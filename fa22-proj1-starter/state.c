#include "state.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Task 1 */
game_state_t* create_default_state() {
	game_state_t *state = (game_state_t *) malloc(sizeof (game_state_t));
	assert(state != NULL);

	// Allocate heap memory for board
	state->board = (char **) malloc(18 * sizeof(char *));
	assert(state->board != NULL);

	for (size_t i = 0; i < 18; i ++) {
		state->board[i]	= (char *) malloc(sizeof ("####################\n"));
		assert(state->board[i] != NULL);
	}

	// Initialize board
	state->num_rows = 18;

	strcpy(state->board[0], "####################\n");
	for (size_t i = 1; i <= 16; i ++) {
		if (i == 2) {
			strcpy(state->board[i], "# d>D    *         #\n");
			continue;
		}
		strcpy(state->board[i], "#                  #\n");
	}
	strcpy(state->board[17], "####################\n");

	// Allocate memory for snake
	state->snakes = (snake_t *) malloc(sizeof (snake_t));
	assert(state->snakes != NULL);

	// Initialize snake
	state->num_snakes = 1;
	state->snakes->head_col = 4;
	state->snakes->head_row = 2;
	state->snakes->tail_col = 2;
	state->snakes->tail_row = 2;
	state->snakes->live = true;

  return state;
}

/* Task 2 */
void free_state(game_state_t* state) {
  // TODO: Implement this function.
	// Free snakes
	free(state->snakes);

	// Free board
	for (size_t i = 0; i < state->num_rows; i ++) {
		free(state->board[i]);
	}
	free(state->board);

	// Free state ptr
	free(state);

  return;
}

/* Task 3 */
void print_board(game_state_t* state, FILE* fp) {
  // TODO: Implement this function.
	for (size_t i = 0; i < state->num_rows; i ++) {
		fputs(state->board[i], fp);
		//fprintf(fp, "\n");
	}
  return;
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
	switch (c) {
		case 'w':
		case 'a':
		case 's':
		case 'd':
			return true;
		default:
			return false;
	}
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
	switch (c) {
		case 'W':
		case 'A':
		case 'S':
		case 'D':
			return true;
		default:
			return false;
	}
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
	char *snake_part = "wasd^<>vWASDx";

	for (size_t i = 0; i < strlen(snake_part); i ++) {
		if (snake_part[i] == c) {
			return true;
		}
	}
  return false;
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
	char *src  = "^<v>";
	char *dest = "wasd";

	for (size_t i = 0; i < 4; i ++) {
		if (c == src[i]) {
			return dest[i];
		}
	}

  return '?';
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
	char *dest = "^<v>";
	char *src  = "WASD";

	for (size_t i = 0; i < 4; i ++) {
		if (c == src[i]) {
			return dest[i];
		}
	}
  return '?';
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
	if (c == 'v' || c == 's' || c == 'S')
		return cur_row + 1;
	else if (c == '^' || c == 'w' || c == 'W')
		return cur_row - 1;
  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
	if (c == '>' || c == 'd' || c == 'D')
		return cur_col + 1;
	else if (c == '<' || c == 'a' || c == 'A')
		return cur_col - 1;
	return cur_col;
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
	snake_t target_snake = state->snakes[snum];
	char target_snake_head = get_board_at(state, target_snake.head_row, target_snake.head_col);

	switch (target_snake_head) {
		case 'W':
			return get_board_at(state, target_snake.head_row - 1, target_snake.head_col);
		case 'A':
			return get_board_at(state, target_snake.head_row, target_snake.head_col - 1);
		case 'S':
			return get_board_at(state, target_snake.head_row + 1, target_snake.head_col);
		case 'D':
			return get_board_at(state, target_snake.head_row, target_snake.head_col + 1);
		default:
			break;
	}

  return '?';
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
	// Get the targets
	snake_t *buf_snake = state->snakes + snum;

	// Modify the board
	char head = get_board_at(state, buf_snake->head_row, buf_snake->head_col);
	switch (head) {
		case 'W':
			set_board_at(state, buf_snake->head_row, buf_snake->head_col, '^');
			set_board_at(state, buf_snake->head_row - 1, buf_snake->head_col, 'W');
			buf_snake->head_row --;
			break;
		case 'A':
			set_board_at(state, buf_snake->head_row, buf_snake->head_col, '<');
			set_board_at(state, buf_snake->head_row, buf_snake->head_col - 1, 'A');
			buf_snake->head_col --;
			break;
		case 'S':
			set_board_at(state, buf_snake->head_row, buf_snake->head_col, 'v');
			set_board_at(state, buf_snake->head_row + 1, buf_snake->head_col, 'S');
			buf_snake->head_row ++;
			break;
		case 'D':
			set_board_at(state, buf_snake->head_row, buf_snake->head_col, '>');
			set_board_at(state, buf_snake->head_row, buf_snake->head_col + 1, 'D');
			buf_snake->head_col ++;
			break;
		default:
			break;
	}

  return;
}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
	// Get the target
	snake_t *buf_snake = state->snakes + snum;
	unsigned next_col = get_next_col(buf_snake->tail_col, get_board_at(state,
																																		 buf_snake->tail_row, buf_snake->tail_col));
	unsigned next_row = get_next_row(buf_snake->tail_row, get_board_at(state,
	                                                                   buf_snake->tail_row, buf_snake->tail_col));

	// Modify the board
	set_board_at(state, next_row, next_col, body_to_tail(get_board_at(state, next_row, next_col)));
	set_board_at(state, buf_snake->tail_row, buf_snake->tail_col, ' ');

	// Update the snake_t
	buf_snake->tail_row = next_row;
	buf_snake->tail_col = next_col;

  return;
}

/* Task 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  // TODO: Implement this function.
	/*
	 * For each snake:
	 * Snakes directly hit into a wall or a snake's body and die
	 * Eat a fruit
	 * Move normally
	 */
	for (unsigned int i = 0; i < state->num_snakes; i ++) {
		snake_t *target_snake = state->snakes + i;
		char next_block = next_square(state, i);

		if (next_block == '#' || is_snake(next_block)) {
			set_board_at(state, target_snake->head_row, target_snake->head_col, 'x');
			target_snake->live = false;
		} else if (next_block == '*') {
			update_head(state, i);
			add_food(state);
		} else {
			update_head(state, i);
			update_tail(state, i);
		}

	}
  return;
}

static unsigned int count_lines(char *filename) {
	FILE *fp;
	unsigned int count = 0;
	char c;

	fp = fopen(filename, "r");

	if (fp == NULL) {
		return 0;
	}

	char buf[2048];

	while (fgets(buf, 2048, fp)) {
		count ++;
	}

	fclose(fp);

	return count;
}

/* Task 5 */
game_state_t* load_board(char* filename) {
  // TODO: Implement this function.
	game_state_t *state = (game_state_t *) malloc(sizeof(game_state_t));
	state->num_rows = count_lines(filename);

	FILE *fp = fopen(filename, "r");
	if (fp == NULL) {
		perror("Error opening file");
		return NULL;
	}

	state->board = (char **)malloc((state->num_rows) * sizeof (char *));

	char buf[2048];

	for (size_t i = 0; i < state->num_rows; i ++) {
		fgets(buf, sizeof(buf), fp);
		state->board[i] = (char *)malloc(strlen(buf) + 1);
		strcpy(state->board[i], buf);
	}

	fclose(fp);

  return state;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.
	unsigned int buf_snum = snum;
	bool find_ans = false;

	for (size_t i = 0; i < state->num_rows; i ++) {
		for (size_t j = 0; j < strlen(state->board[i]); j ++) {
			if (is_head(get_board_at(state, i, j))) {
				if (buf_snum == 0)	 {
					(state->snakes + snum)->head_row = i;
					(state->snakes + snum)->head_col = j;
					find_ans = true;
					break;
				}
				buf_snum --;
			}
		}

		if (find_ans)
			break;
	}

  return;
}

static void find_tail(game_state_t* state, unsigned int snum) {
	// TODO: Implement this function.
	unsigned int buf_snum = snum;
	bool find_ans = false;

	for (size_t i = 0; i < state->num_rows; i ++) {
		for (size_t j = 0; j < strlen(state->board[i]); j ++) {
			if (is_tail(get_board_at(state, i, j))) {
				if (buf_snum == 0)	 {
					(state->snakes + snum)->tail_row = i;
					(state->snakes + snum)->tail_col = j;
					find_ans = true;
					break;
				}
				buf_snum --;
			}
		}
		if (find_ans) {
			break;
		}
	}

	return;
}

/* Task 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
  // TODO: Implement this function.
	unsigned int count = 0;

	for (size_t i = 0; i < state->num_rows; i ++) {
		for (size_t j = 0; j < strlen(state->board[i]); j ++) {
			if (is_head(get_board_at(state, i, j))) {
				count ++;
			}
		}
	}

	state->num_snakes = count;
	state->snakes = (snake_t *) malloc(count * sizeof(snake_t));

	for (size_t i = 0; i < count; i ++)	{
		find_head(state, i);
		find_tail(state, i);
	}

  return state;
}

/*
 * This implements the setting up of a words
 * game in which the players have to pick
 * the right words in a list with only two
 * hints provided by teammates:
 * -
 *
 */
#include "stdlib.h"
#include "stdio.h"
#include "time.h"
#include "assert.h"

#define C_W 25
#define I_TABOO 0
#define C_TABOO 1
#define I_NEUTRAL (I_TABOO+C_TABOO)
#define C_NEUTRAL 9
#define I_T1 (I_NEUTRAL+C_NEUTRAL)
#define C_T1 8
#define I_T2 (I_T1+C_T1)
#define C_T2 7

#define Mo (1024*1024)

#define WORDLIST_PATH "wordlist.txt"

int randint(int upper_boundary) {
	// 0 <= return value < upper_boundary
	return rand() % upper_boundary;
}

void shuffle_array(int *arr, int count) {
	// Fisher-Yates
	int swapper;
	int rnd;
	for (int n = count - 1;
			n >= 1;
			n--) {
		rnd = randint(n+1);
		swapper = arr[n];
		arr[n] = arr[rnd];
		arr[rnd] = swapper;
	}
}

int fsize(FILE *f) {
	// returns the size of an open file
	assert(f!=NULL);
	int ret;
	fseek(f, 0L, SEEK_END);
	ret = ftell(f);
	fseek(f, 0L, SEEK_SET);
	return ret;
}

typedef struct {
	// representation of the game data
	char *deck_words_data;
	int *deck_words;
	int deck_words_count;
	// helper pointers: they point inside
	// the deck_words array.
	int *board, *taboo, *t1, *t2, *neutral;
	int shuffled[C_W];
} Game;

int game_init(Game* game);
int game_free(Game* game);
int game_load_deck(Game* game);

int game_init(Game* game) {
	game_load_deck(game);
	shuffle_array(game->deck_words,
			game->deck_words_count);
  for (int i = 0; i < C_W; i++) {
		game->shuffled[i] = game->deck_words[i];
	}
	shuffle_array(game->shuffled, C_W);
	game->board = game->deck_words;
	game->taboo = game->board + I_TABOO;
	game->t1 = game->board + I_T1;
	game->t2 = game->board + I_T2;
	game->neutral = game->board + I_NEUTRAL;
	return 1;
}

int game_free(Game* game) {
	free(game->deck_words_data);
	free(game->deck_words);
	return 1;
}

int game_load_deck(Game* game) {
	FILE *f = fopen(WORDLIST_PATH, "r");
	int filesize = fsize(f);
	int cur_word_index = 0;
	int deck_words_count = 0;
	char c;
	game->deck_words_data =
		(char*) calloc(filesize+1, sizeof(char));
	game->deck_words = 
		(int*) malloc( filesize * sizeof(int));
	assert(game->deck_words_data != NULL);
	for (int n = 0; n < filesize; n++) {
		c = fgetc(f);
		switch(c) {
			case '\n':
				game->deck_words[deck_words_count++] =
					cur_word_index;
				cur_word_index = n+1;
				break;
			default:
				game->deck_words_data[n] = c;
				break;
		}
	}
	fclose(f);
	if (c == '\n') {
		deck_words_count--;
	}
	game->deck_words =
		(int*) realloc(
				game->deck_words,
				deck_words_count * sizeof(int));
	game->deck_words_count = deck_words_count;
	return 1;
}

int game_print(Game* game) {
	printf("All words:\n");
	for (int i = 0; i < C_W; i++) {
		printf("\t%s\n",
				game->deck_words_data +
					game->shuffled[i]);
	}
	int color = 33;
	for (int i = 0; i < C_W; i++) {
		if (i == I_TABOO) color = 36;
		if (i == I_NEUTRAL) color = 33;
		if (i == I_T1) color = 31;
		if (i == I_T2) color = 34;
		printf("  \033[%dm%s\033[0m\n",
				color,
				game->deck_words_data +
				  game->board[i]);
	}
	
	return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
	Game game;
	srand(time(NULL));

	game_init(&game);
	
	game_print(&game);
	game_free(&game);
}

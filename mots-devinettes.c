/*
 * C implementation by:
 *    Bzhu-Noors
 *
 * Word list by:
 *    Bzhu-Noors
 *
 * Inspired by Codenames, by:
 *    Vlaada Chvátil
 *
 * Notice: this C program does not implement
 * the "business logic" of the original game:
 * it only helps setting it up.
 *
 * You are therefore not bound to any set of
 * rules.
 *
 * */
#include "stdlib.h"
#include "stdio.h"
#include "time.h"
#include "assert.h"
#include "string.h"

#define C_W 25
#define I_TABOO 0
#define C_TABOO 1
#define I_NEUTRAL (I_TABOO+C_TABOO)
#define C_NEUTRAL 9
#define I_T1 (I_NEUTRAL+C_NEUTRAL)
#define C_T1 8
#define I_T2 (I_T1+C_T1)
#define C_T2 7
#define STRIKECOL 45

#define Mo (1024*1024)

#define WORDLIST_PATH "wordlist.txt"

/**
 * Returns a pseudo-random number between 0 and
 * upper_boundary (not included).
 * Note that the distribution will not be totally
 * homogenous because we simply use modulo to
 * coerce the random number to the desired range.
 */
int randint(int upper_boundary) {
	// 0 <= return value < upper_boundary
	return rand() % upper_boundary;
}

int len_utf(char *s) {
	int l,i;
	while (l < 200 && s[i]) {
		if ((unsigned char)s[i] != 195) l++;
	}
	return l;
}

char* pad20(char* s) {
	// non-generic, pad to 20 chars + \0
	// make room for non-ascii characters
	char *ret = (char*)malloc(41);
	int pad = 0;
	int i;
	int mblen = 0;
	unsigned char c = 1;
	for (i = 0; mblen < 18 && i < 40; i++) {
		c = (unsigned char) s[i];
		ret[i] = (char) c;
		switch (c) {
			case 0:
				pad=1;
				mblen++;
				break;
			case 195:
				break;
			case 197:
				break;
			default:
				mblen++;
				break;
		}
		ret[i] = pad ? '.' : s[i];
	}
	ret[i] = 0;
	return ret;
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
	int striked_words[C_W];
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
	for (int i = 0; i < C_W; i++) {
		game->striked_words[i] = 0;
	}
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

/**
 * Frees the memory used by game data.
 */
int game_free(Game* game) {
	free(game->deck_words_data);
	free(game->deck_words);
	return 1;
}

/**
 * Loads the word list from an external file.
 */
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

/**
 * Maps terminal colors to word indices.
 */
int get_color(int word_index) {
	int ret;
	if (word_index >= I_TABOO) ret = 36;
	if (word_index >= I_NEUTRAL) ret = 33;
	if (word_index >= I_T1) ret = 31;
	if (word_index >= I_T2) ret = 34;
	return ret;
}

/**
 * This function displays the current game board.
 * The words are displayed differently depending on
 * whether the player is a guesser or a “knower”.
 */
int game_print(Game* game, char blind) {
	int color[2] = {37,37};
	char w[2][20];
	char *pw1, *pw2;
	printf("\033[2J");
	printf("All words:\n");
	for (int i = 0; i < C_W; i+=2) {
		for (int col = 0; col < 2; col++){
			if (i+col >= C_W) {
				w[col][0]=0;
				color[col] = 30;
				continue;
			}
			color[col] = game->striked_words[i+col]?
				STRIKECOL : (blind ? 
						37 : get_color(i+col));
			strcpy(
					w[col],
					game->deck_words_data +
						(blind ?
							game->shuffled[i+col]
							: game->board[i+col])
			);
		}
		pw1 = pad20(w[0]);
		pw2 = pad20(w[1]);
		printf("%2d \033[%dm%s\033[0m"
				" %2d \033[%dm%s\033[0m\n",
				i+1,
				color[0],
				pw1,
				i+2,
				color[1],
				pw2);
		free(pw1);
		free(pw2);
	}
	
	return 1;
}

/**
 * This function returns a number entered by the player.
 */
int get_code_from_user() {
	char str_code[10];
	scanf("%3s", str_code);
	return atoi(str_code);
}

/**
 * This returns true when all the words have been striked out.
 */
int game_is_finished(Game *game) {
	int i;
	for (i = 0; i < C_W; i++) {
		if (game->striked_words[i] == 0)
			return 0;
	}
	return 1;
}

/**
 * this function initializes the pseudo-random number generator
 * using the current time modulo 5 minutes. That way, each player
 * can play on their own smarphones: even with no network at all,
 * the games will draw the same words for all players.
 */
long int randomize_5min() {
	long int seconds = (long int) time(NULL);
	// 5 minutes
	srand(seconds / 300);
	return seconds / 300;
}

int main(int argc, char *argv[]) {
	Game game;
	int code;
	char blind;
	char response_1=0;
	char response_2[2];
	// num will store the index of the next word to be striked
	int num = 0;
	randomize_5min();
	while(
			response_1 != 'o' &&
			response_1 != 'n') {
		puts("Êtes-vous du côté aveugle (o/n)?");
		scanf("%1s", &response_1);
	}
	switch(response_1) {
		case 'o':
			blind=1;
			break;
		case 'n':
			blind=0;
			break;
	}

	game_init(&game);

	while(!game_is_finished(&game)) {
	game_print(&game, blind);
		puts("Please enter the index of a word"
				" (or 0 to exit).");
		num = get_code_from_user();
		if (num == 0) break;
		if (num > C_W
				|| num < 1) {
			continue;
		}
		num--;
		game.striked_words[num] =
			!game.striked_words[num];
	}
	game_free(&game);
	return EXIT_SUCCESS;
}

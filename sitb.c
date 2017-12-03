#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>

#define DIMENSIONS_MAX 31

typedef struct {
	uint32_t choice;
	uint32_t choices_idx;
	uint32_t dimensions_max;
	uint32_t valid;
}
roll_t;

typedef struct {
	uint32_t choice;
}
unroll_t;

typedef enum {
	CALL_ROLL,
	CALL_UNROLL
}
call_t;

int add_roll(uint32_t, uint32_t, uint32_t, uint32_t);
void set_roll(roll_t *, uint32_t, uint32_t, uint32_t, uint32_t);
int add_unroll(uint32_t);
void set_unroll(unroll_t *, uint32_t);
int add_call(call_t);
int perform_call(call_t *);
int perform_roll(uint32_t, uint32_t, uint32_t, uint32_t);
int perform_unroll(uint32_t);
void print_path(int);
void print_choice(const char *, uint32_t);

int verbose, *marks;
unsigned time0;
uint32_t dimensions_n, calls_max, calls_size, rolls_max, rolls_size, unrolls_max, unrolls_size, choices_max, *choices;
uint64_t nodes;
roll_t *rolls;
unroll_t *unrolls;
call_t *calls;

int main(void) {
	if (scanf("%" SCNu32 "%d", &dimensions_n, &verbose) != 2 || dimensions_n < 1 || dimensions_n > DIMENSIONS_MAX) {
		fprintf(stderr, "Invalid number of dimensions\n");
		return EXIT_FAILURE;
	}
	uint32_t marks_n = 1U << dimensions_n;
	marks = calloc((size_t)marks_n, sizeof(int));
	if (!marks) {
		fprintf(stderr, "Could not allocate memory for marks\n");
		return EXIT_FAILURE;
	}
	calls_max = 0;
	calls_size = 0;
	rolls_max = 0;
	rolls_size = 0;
	unrolls_max = 0;
	unrolls_size = 0;
	choices_max = 0;
	nodes = 0;
	time0 = (unsigned)time(NULL);
	marks[0]++;
	if (!add_roll(0, 0, 1, marks_n-1)) {
		free(marks);
		return EXIT_FAILURE;
	}
	int r;
	do {
		calls_size--;
		r = perform_call(calls+calls_size);
	}
	while (r && calls_size > 0);
	marks[0]--;
	print_path(1);
	if (choices_max > 0) {
		free(choices);
	}
	if (unrolls_max > 0) {
		free(unrolls);
	}
	if (rolls_max > 0) {
		free(rolls);
	}
	if (calls_max > 0) {
		free(calls);
	}
	free(marks);
	return EXIT_SUCCESS;
}

int add_roll(uint32_t choice, uint32_t choices_idx, uint32_t dimensions_max, uint32_t valid) {
	if (!add_call(CALL_ROLL)) {
		return 0;
	}
	if (rolls_size == rolls_max) {
		if (rolls_max == 0) {
			rolls = malloc(sizeof(roll_t));
			if (!rolls) {
				fprintf(stderr, "Could not allocate memory for rolls\n");
				return 0;
			}
		}
		else {
			roll_t *rolls_tmp = realloc(rolls, sizeof(roll_t)*(rolls_max+1));
			if (!rolls_tmp) {
				fprintf(stderr, "Could not reallocate memory for rolls\n");
				return 0;
			}
			rolls = rolls_tmp;
		}
		rolls_max++;
	}
	set_roll(rolls+rolls_size, choice, choices_idx, dimensions_max, valid);
	rolls_size++;
	return 1;
}

void set_roll(roll_t *roll, uint32_t choice, uint32_t choices_idx, uint32_t dimensions_max, uint32_t valid) {
	roll->choice = choice;
	roll->choices_idx = choices_idx;
	roll->dimensions_max = dimensions_max;
	roll->valid = valid;
}

int add_unroll(uint32_t choice) {
	if (!add_call(CALL_UNROLL)) {
		return 0;
	}
	if (unrolls_size == unrolls_max) {
		if (unrolls_max == 0) {
			unrolls = malloc(sizeof(unroll_t));
			if (!unrolls) {
				fprintf(stderr, "Could not allocate memory for unrolls\n");
				return 0;
			}
		}
		else {
			unroll_t *unrolls_tmp = realloc(unrolls, sizeof(unroll_t)*(unrolls_max+1));
			if (!unrolls_tmp) {
				fprintf(stderr, "Could not reallocate memory for unrolls\n");
				return 0;
			}
			unrolls = unrolls_tmp;
		}
		unrolls_max++;
	}
	set_unroll(unrolls+unrolls_size, choice);
	unrolls_size++;
	return 1;
}

void set_unroll(unroll_t *unroll, uint32_t choice) {
	unroll->choice = choice;
}

int add_call(call_t call) {
	if (calls_size == calls_max) {
		if (calls_max == 0) {
			calls = malloc(sizeof(call_t));
			if (!calls) {
				fprintf(stderr, "Could not allocate memory for calls\n");
				return 0;
			}
		}
		else {
			call_t *calls_tmp = realloc(calls, sizeof(call_t)*(calls_max+1));
			if (!calls_tmp) {
				fprintf(stderr, "Could not reallocate memory for calls\n");
				return 0;
			}
			calls = calls_tmp;
		}
		calls_max++;
	}
	calls[calls_size++] = call;
	return 1;
}

int perform_call(call_t *call) {
	int r;
	switch (*call) {
	case CALL_ROLL:
		rolls_size--;
		r = perform_roll(rolls[rolls_size].choice, rolls[rolls_size].choices_idx, rolls[rolls_size].dimensions_max, rolls[rolls_size].valid);
		break;
	case CALL_UNROLL:
		unrolls_size--;
		r = perform_unroll(unrolls[unrolls_size].choice);
		break;
	default:
		fprintf(stderr, "Invalid call\n");
		fflush(stderr);
		r = 0;
	}
	return r;
}

int perform_roll(uint32_t choice, uint32_t choices_idx, uint32_t dimensions_max, uint32_t valid) {
	nodes++;
	if (choices_idx+valid < choices_max) {
		return 1;
	}
	if (choices_idx == choices_max) {
		if (choices_max == 0) {
			choices = malloc(sizeof(uint32_t));
			if (!choices) {
				fprintf(stderr, "Could not allocate memory for choices\n");
				return 0;
			}
		}
		else {
			uint32_t *choices_tmp = realloc(choices, sizeof(uint32_t)*(choices_max+1));
			if (!choices_tmp) {
				fprintf(stderr, "Could not reallocate memory for choices\n");
				return 0;
			}
			choices = choices_tmp;
		}
		choices[choices_max++] = choice;
		print_path(0);
	}
	else {
		choices[choices_idx] = choice;
	}
	uint32_t weight, i;
	for (weight = 1, i = 0; i < dimensions_n; weight <<= 1, i++) {
		if (choice & weight) {
			marks[choice-weight]++;
			if (marks[choice-weight] == 1) {
				valid--;
			}
		}
		else {
			marks[choice+weight]++;
			if (marks[choice+weight] == 1) {
				valid--;
			}
		}
	}
	if (!add_unroll(choice)) {
		return 0;
	}
	weight = 1U << (dimensions_max-1);
	if (dimensions_max < dimensions_n && (choice & weight)) {
		dimensions_max++;
		weight <<= 1;
	}
	for (i = 0; i < dimensions_max; weight >>= 1, i++) {
		if (choice & weight) {
			if (marks[choice-weight] == 1 && !add_roll(choice-weight, choices_idx+1, dimensions_max, valid)) {
				return 0;
			}
		}
		else {
			if (marks[choice+weight] == 1 && !add_roll(choice+weight, choices_idx+1, dimensions_max, valid)) {
				return 0;
			}
		}
	}
	return 1;
}

int perform_unroll(uint32_t choice) {
	uint32_t weight, i;
	for (weight = 1, i = 0; i < dimensions_n; weight <<= 1, i++) {
		if (choice & weight) {
			marks[choice-weight]--;
		}
		else {
			marks[choice+weight]--;
		}
	}
	return 1;
}

void print_path(int complete) {
	if (verbose || complete) {
		printf("%" PRIu32, dimensions_n);
		print_choice(" = ", choices[0]);
		uint32_t i;
		for (i = 1; i < choices_max; i++) {
			print_choice(" -> ", choices[i]);
		}
		puts("");
	}
	printf("Length %" PRIu32 " Nodes %" PRIu64 " Time %us\n", choices_max-1, nodes, (unsigned)time(NULL)-time0);
	fflush(stdout);
}

void print_choice(const char *prefix, uint32_t choice) {
	uint32_t weight, i;
	printf("%s", prefix);
	for (weight = 1, i = 0; i < dimensions_n; weight <<= 1, i++) {
		if (choice & weight) {
			putchar('1');
		}
		else {
			putchar('0');
		}
	}
}

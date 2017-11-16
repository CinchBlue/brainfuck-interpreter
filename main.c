#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int parse_unsigned(unsigned * const result) {
  int index = 0;
  const int MAX_LIM = 20;
  /*
   * A 64-bit unsigned number has a max length
   * of 20 decimal digits, and thus 20 characters.
   */
  char str[MAX_LIM + 1];
  str[index] = (char) getchar();
  if (!isdigit(str[index])) {
    return 0;
  }
  while (isdigit(str[index])) {
    ++index;
    if (index >= MAX_LIM) {
      return 0;
    }
    str[index] = (char) getchar();
  }
  str[index] = '\0';
  *result = strtoul(str, 0L, 10);
  return 1;
}

typedef struct {
  char * program;
  unsigned program_index;
  unsigned program_size;

  char * memory;
  unsigned size;
  unsigned index;
  unsigned depth;

  char * output;
  unsigned output_size;
  unsigned output_index;
} BFState;

void print_BFState(const BFState * const state) {
  unsigned start_index = state->index;
  if (state->index < 8) {
    start_index = 8;
  }
  if (state->index > state->size - 9) {
    start_index = state->size - 9;
  }
  putchar('|');
  for (unsigned i = start_index - 8; i < start_index + 8; ++i) {
    printf("%3u|", (unsigned char) state->memory[i]);
  }
  printf("\n");
  putchar(' ');
  for (unsigned i = start_index - 8; i < start_index + 8; ++i) {
    if (i == state->index) {
      printf("^^^ ");
    } else {
      printf("%3u ", i);
    }
  }
  printf("\n");
}

int is_BF_character(char c) {
  return c == '<' ||
      c == '>' ||
      c == '.' ||
      c == ',' ||
      c == '-' ||
      c == '+' ||
      c == '[' ||
      c == ']';
}

const char * process_BF_command(BFState * const state) {
  char c = state->program[state->program_index];
  while (isspace(c)) {
      state->program_index++;
      c = state->program[state->program_index];
  }
  printf("[%u]: %c\n",
         (unsigned) state->program_index,
         c);
  if (c == '\0') { return NULL; }
  switch (c) {
    case '<':
      if (state->index == 0) {
        return "cannot go under index 0";
      }
      state->index--;
      state->program_index++;
      break;
    case '>':
      if (state->index >= state->size) {
        return "cannot go over index MAX_INDEX";
      }
      state->index++;
      state->program_index++;
      break;
    case '-':
      state->memory[state->index]--;
      state->program_index++;
      break;
    case '+':
      state->memory[state->index]++;
      state->program_index++;
      break;
    case ',':
      state->memory[state->index] = getchar();
      state->program_index++;
      break;
    case '.':
      //putchar((char) state->memory[state->index]);
      state->output[state->output_index++] = state->memory[state->index];
      if (state->output_index >= state->output_size) {
        void* temp =  realloc((void*) state->output, state->output_size*2);
        if (!temp) {
          return "could not reallocate enough memory for output";
        }
        state->output = temp;
      }
      state->program_index++;
      break;
    case '[':
      state->depth++;
      if (state->memory[state->index] == 0) {
        while (state->program[state->program_index] != ']') {
          if (state->program_index >= state->program_size) {
            return "unbalanced [";
          }
          state->program_index++;
        }
      } else {
        state->program_index++;
      }
      break;
    case ']':
      if (state->depth == 0) {
        return "cannot begin unbalanced block with ]";
      }
      state->depth--;
      if (state->memory[state->index] != 0) {
        while (state->program[state->program_index] != '[') {
          if (state->program_index == 0) {
            return "detected unbalanced ]";
          }
          state->program_index--;
        }
      } else {
        state->program_index++;
      }
      break;
    default: {
      return "invalid BF instruction";
    }
  };
  return NULL;
}

int init_BFState (BFState* state) {
  void* temp = calloc(state->size, sizeof(char));
  if (!temp) { return 0; }
  state->memory = temp;
  state->index = 0;
  state->depth = 0;
  temp = calloc(65536, sizeof(char));
  if (!temp) { return 0; }
  state->program = temp;
  fgets((char *) state->program, 65536, stdin);
  puts(state->program);
  state->program_index = 0;
  state->program_size = strlen((char *) state->program);
  state->output_size = 128;
  state->output_index = 0;
  temp = calloc(state->output_size, sizeof(char));
  if (!temp) { return 0; }
  state->output = temp;
  return 1;
}

int main() {
  BFState state;
  printf("Input memory size: ");
  if (!parse_unsigned(&state.size)) {
    return 1;
  }
  if (!init_BFState(&state)) {
    return 4;
  }
  print_BFState(&state);
  while (state.program_index < state.program_size &&
      state.program[state.program_index] != '\0' &&
      state.program[state.program_index] != EOF) {
    //printf("%u\n", (unsigned) state->program[state.program_index]);

    const char * error = process_BF_command(&state);
    if (error) {
      printf("ERROR: %s", error);
      return 2;
    }
    if (state.program_index >= state.program_size) {
      printf("FINAL STATE:");
    }
    puts("");
    print_BFState(&state);
  }
  void* temp = realloc((void*) state.output, state.output_size+1);
  if (!temp) {
    printf("Could not reallocate enough memory at end for output");
    return 3;
  }
  state.output = temp;
  state.output_size = state.output_size+1;
  state.output[state.output_size-1] = '\0';
  printf("FINAL OUTPUT:\n%s\n", state.output);
  free(state.program);
  free(state.memory);
  return 0;
}
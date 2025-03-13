#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define clear() system("@cls||clear")
#else
#define clear() printf("\033[H\033[J")
#endif

#define LINE_LEN 100


enum terminalStates {
  ACCEPT_STATE = 0,
  DECLINE_STATE,
  ERROR_STATE,
};

typedef struct MachineState {
  char *name;
  char *output;
  int *next;
  int *move;
} State;

typedef struct MachineStates {
  State *states;
  int len;
  int cap;
} States;

typedef struct Tape {
  char *cells;
  int len;
  int cap;
} Tape;

struct TuringMachine {
  int cell;
  unsigned int currState;
} DTM;

void readProgram(char *path, char **alphabet, Tape *tape, States *states);
void loadState(State *state, char *state_outputs, char *state_moves, char *state_next, char *state_name, int n);
void loadTape(Tape *tape, char *buf, char empty);
void loadAlphabet(char **alphabet, char *buf);
void turingLoop(char *alphabet, States *states, Tape *tape);
int resolveTM(char *alphabet, States *states, Tape *tape);
void printTape(Tape *tape);
char* printLine(int line, char* buf, Tape *tape);
char* printMarker(int line, int marked, char* buf);
void initStates(States *states);
void freeStates(States *states);
void extendCapTape(Tape *tape, char empty);
void extendCapStates(States *states);

int main(int argc, char **argv) {
  char *alphabet;
  Tape tape;
  States states;
  DTM.currState = 3;
  DTM.cell = 0;

  if (argc != 2) {
    printf("Usage: %s [path_to_program]\n", argv[0]);
    exit(1);
  }

  initStates(&states);
  readProgram(argv[1], &alphabet, &tape, &states);
  printf("Alphabet: %s\n", alphabet);
  for (int i = 0; i < states.len; i++) {
    printf("%2d: %s\n", i, states.states[i].name);
    if (i <= ERROR_STATE)
      continue;
    printf("ALPHAB: ");
    for (int j = 0; j < (int)strlen(alphabet); j++) {
      printf("%c ", alphabet[j]);
  }

    printf("\nOUTPUT: ");
    for (int j = 0; j < (int)strlen(alphabet); j++) {
      if (states.states[i].next != NULL) printf("%c ", states.states[i].output[j]);
    }

    printf("\n  NEXT: ");
    for (int j = 0; j < (int)strlen(alphabet); j++) {
      if (states.states[i].next != NULL) printf("%d ", states.states[i].next[j]);
    }

    printf("\n  MOVE: ");
    for (int j = 0; j < (int)strlen(alphabet); j++) {
      if (states.states[i].move != NULL) printf("%d ", states.states[i].move[j]);
    }
    printf("\n");
  }
  printf("\n%s\n", tape.cells);
  printf("\nPress Enter to run program.\n");
  fgetc(stdin);
  turingLoop(alphabet, &states, &tape);
  
  printf("\n\nTuring machine terminated...\n");
  printf("\nExit code: %d ", DTM.currState);
  if (states.states[DTM.currState].name != NULL)
    printf("[%s]\n", states.states[DTM.currState].name);
  else
    printf("\n");


  if (alphabet != NULL)
    free(alphabet);
  if (tape.cells != NULL)
    free(tape.cells);
  if (states.states != NULL)
    freeStates(&states);
  return 0;
}

int resolveTM(char *alphabet, States *states, Tape *tape) {
  State *state = (states->states + DTM.currState);
  int value = 0;
  if (DTM.cell < 0) {
      DTM.cell = 0;
      DTM.currState = ERROR_STATE;
      return 0;
  }
  if (DTM.cell >= tape->cap)
    extendCapTape(tape, alphabet[0]);
  while (tape->cells[DTM.cell] != alphabet[value]) {
    value++;
    if (value >= (int)strlen(alphabet)) {
      DTM.currState = ERROR_STATE;
      return 0;
    }
  }

  int output = state->output[value];
  int move = state->move[value];
  int nextState = state->next[value];

  if (DTM.cell + 1 >= tape->cap)
    extendCapTape(tape, alphabet[0]);
  if ((DTM.cell + move) < 0) {
    DTM.currState = ERROR_STATE;
    return 0;
  }
  tape->cells[DTM.cell] = output;
  DTM.cell += move;
  DTM.currState = nextState;
  if (nextState <= ERROR_STATE)
    return 0;
  return 1;
}

void turingLoop(char *alphabet, States *states, Tape *tape) {
  int run = 1;
  char skip = '\n';
  while (run) {
    printTape(tape);
    printf("Current state: %d ", DTM.currState);
    if (states->states[DTM.currState].name != NULL)
      printf("[%s]\n", states->states[DTM.currState].name);
    else
      printf("\n");

    if (skip != 'r') { 
      printf("\nPress Enter to step forward. Or type 'r' to run whole program.\n");
      skip = fgetc(stdin);
    }
    run = resolveTM(alphabet, states, tape);
  }
}

void printTape(Tape *tape) {
  clear();
  char *buf = (char*)calloc(tape->cap*4, sizeof(char));
  char *tmp = buf;
  int i = 0;
  while (i < (tape->cap / LINE_LEN) + 1) {
    tmp += snprintf(tmp, 2, "\n");
    tmp = printLine(i, tmp, tape);

    if ((DTM.cell / LINE_LEN) == i) {
      tmp = printMarker(i, DTM.cell, tmp);
    } else {
      tmp += snprintf(tmp, 2, "\n");
    }
    
    i++;
  } 
  puts(buf);
  free(buf);
}

char* printLine(int line, char* buf, Tape *tape) {
  for (int i = line * LINE_LEN; i < (line * LINE_LEN + LINE_LEN) && i < tape->cap; i++) {
    buf += snprintf(buf, 3, "%c ", tape->cells[i]);
  }
  return buf;
}

char* printMarker(int line, int marked, char* buf) {
  int l = 0;
  int tmp = marked;
  while (tmp != 0) {
    l++;
    tmp >>= 1;
  }
  buf += snprintf(buf, 2, "\n");
  marked = marked - (line * LINE_LEN);
  for (int i = 0; i < marked; i++) {
    buf += snprintf(buf, 3, "  ");
  }
  buf += snprintf(buf, 6 + l, "^ [%d]", marked);
  return buf;
}


void initStates(States *states) {
    if ((states->states = (State*) malloc(sizeof(State) * 4)) == NULL) {
        perror("malloc");
        exit(1);
    }
    states->cap = 4;
    states->len = 3;
    (states->states)->name = (char*) malloc(sizeof(char) * 16);
    ((states->states) + 1)->name = (char*) malloc(sizeof(char) * 16);
    ((states->states) + 2)->name = (char*) malloc(sizeof(char) * 16);
    strcpy((states->states)->name, "ACCEPT");
    strcpy(((states->states) + 1)->name, "DECLINE");
    strcpy(((states->states) + 2)->name, "ERROR");
}

void loadState(State *state, char *state_output, char *state_move,
               char *state_next, char *state_name, int n) {
  char *next = NULL;
  int i = 0;
  state->name = NULL;
  state->next = NULL;
  state->move = NULL;
  state->output = NULL;

  next = strtok(state_next, " ");
  if ((state->next = (int *)malloc(sizeof(int) * n)) == NULL) {
    perror("malloc");
    exit(1);
  }
  do {
    state->next[i] = atoi(next);
    i++;
  } while ((next = strtok(NULL, " ")) != NULL && i < n);
  while (i < n) {
    state->next[i] = ERROR_STATE;
    i++;
  }

  i = 0;
  next = strtok(state_output, " ");
  if ((state->output = (char *)malloc(sizeof(char) * n)) == NULL) {
    perror("malloc");
    exit(1);
  }
  do {
    memcpy(state->output + i, next, sizeof(char));
    i++;
  } while ((next = strtok(NULL, " ")) != NULL && i < n);
  while (i < n) {
    memcpy(state->output + i, " ", sizeof(char));
    i++;
  }

  i = 0;
  next = strtok(state_move, " ");
  if ((state->move = (int *)malloc(sizeof(int) * n)) == NULL) {
    perror("malloc");
    exit(1);
  }

  do {
    state->move[i] = atoi(next);
    i++;
  } while ((next = strtok(NULL, " ")) != NULL && i < n);
  while (i < n) {
    state->move[i] = 0;
    i++;
  }

  if (state_name != NULL) {
    state->name = malloc(strlen(state_name) * sizeof(char));
    memcpy(state->name, state_name, strlen(state_name));
  }
}

void loadTape(Tape *tape, char *buf, char empty) {
  char *cells = (char *)calloc(1, sizeof(char));
  tape->cells = cells;
  tape->len = 0;
  tape->cap = 1;

  for (int i = 0; i < (int)strlen(buf); i++) {
    if (i >= tape->cap) {
      extendCapTape(tape, empty);
    }
    if (buf[i] != '\n') {
      tape->cells[i] = buf[i];
      tape->len++;
    }
  }
}

void loadAlphabet(char **alphabet, char *buf) {
  (*alphabet) = (char *)malloc(strlen(buf) * sizeof(char));
  int i;
  for (i = 0; i < (int)strlen(buf); i++) {
    (*alphabet)[i] = buf[i];
  }
  (*alphabet)[i-1] = '\0';

}

void readProgram(char *path, char **alphabet, Tape *tape, States *states) {
  // name, next, move, out 
  FILE *program;
  if ((program = fopen(path, "r")) == NULL) {
    perror("Error while opening program file");
    exit(1);
  }
  char *buf = (char *)malloc(4096 * sizeof(char));
  char *state_move = NULL;
  char *state_output = NULL;
  char *state_next = NULL;
  char *state_name = NULL;

  int i = 0;
  while (fgets(buf, 4096, program)) {
    if (i == 0) {
      loadAlphabet(alphabet, buf);
    } else if (i == 1) {
      loadTape(tape, buf, *(alphabet[0]));
    } else {
      switch ((i - 2) % 4) {
      case 0:
        if (buf[0] == '\n') {
          state_name = NULL;
          break;
        }
        state_name = (char *)malloc(strlen(buf) * sizeof(char));
        if (state_name == NULL) {
          perror("Couldn't malloc string");
          exit(1);
        }
        memcpy(state_name, buf, strlen(buf));
        state_name[strlen(buf)-1] = '\0';
      break;
      case 1:
        state_next = (char *)malloc(strlen(buf) * sizeof(char));
        if (state_next == NULL) {
          perror("Couldn't malloc string");
          exit(1);
        }
        memcpy(state_next, buf, strlen(buf));
        state_next[strlen(buf)-1] = '\0';
        break;
      case 2:
        state_move = (char *)malloc(strlen(buf) * sizeof(char));
        if (state_move == NULL) {
          free(state_next);
          perror("Couldn't malloc string");
          exit(1);
        }
        memcpy(state_move, buf, strlen(buf));
        state_move[strlen(buf)-1] = '\0';
        break;
      case 3:
        state_output = (char *)malloc(strlen(buf) * sizeof(char));
        if (state_output == NULL) {
          free(state_next);
          free(state_move);
          perror("Couldn't malloc string");
          exit(1);
        }
        memcpy(state_output, buf, strlen(buf));
        state_output[strlen(buf)-1] = '\0';

        if (states->len >= states->cap)
          extendCapStates(states);

        loadState(states->states + ((i - 2) / 4) + 3, state_output, state_move,
                  state_next, state_name, strlen(*alphabet));
        states->len++;

        if (state_next != NULL) {
          free(state_next);
          state_next = NULL;
        }
        if (state_move != NULL) {
          free(state_move);
          state_move = NULL;
        }
        if (state_output != NULL) {
          free(state_output);
          state_output = NULL;
        }
        if (state_name != NULL) {
          free(state_name);
          state_name = NULL;
        }
        break;
      }
    }
    i++;
  }
  if (state_next != NULL) {
    free(state_next);
    state_next = NULL;
  }
  if (state_move != NULL) {
    free(state_move);
    state_move = NULL;
  }
  if (state_output != NULL) {
    free(state_output);
    state_output = NULL;
  }
  if (state_name != NULL) {
    free(state_name);
    state_name = NULL;
  }

  fclose(program);
  free(buf);
}

void freeState(State *state) {
  if (state->name != NULL) free(state->name);
  free(state->move);
  free(state->next);
  free(state->output);
}

void freeStates(States *states) {
  for (int i = 0; i < states->len; i++) {
    freeState(states->states + i);
  }
}

void extendCapStates(States *states) {
  states->cap *= 2;
  State *p;
  if ((p = realloc(states->states, sizeof(State) * states->cap)) == NULL) {
    perror("Couldn't double states capacity");
    freeStates(states);
    exit(1);
  }
  states->states = p;
}

void extendCapTape(Tape *tape, char empty) {
  tape->cap *= 2;
  char *p;
  if ((p = realloc(tape->cells, sizeof(char) * tape->cap)) == NULL) {
    perror("Couldn't double tape capacity");
    free(tape->cells);
    exit(1);
  }
  tape->cells = p;
  
  for (int i = tape->len + 1; i < tape->cap; i++) {
    tape->cells[i] = empty; // change to use default empty char
  }
}

#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Al inicio del archivo, después de los includes
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L // Para strdup en sistemas modernos
#endif

#include <string.h>

// Si strdup no está disponible, proporciona una implementación
#ifndef HAVE_STRDUP
char* strdup(const char* s) {
    size_t len = strlen(s) + 1;
    char* dup = malloc(len);
    if (dup) {
        memcpy(dup, s, len);
    }
    return dup;
}
#endif

#include "snake_utils.h"

// Definiciones de funciones de ayuda.
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

/* Tarea 1 */
game_state_t* create_default_state() {
  game_state_t* state = malloc(sizeof(game_state_t));
    if (!state) return NULL;

    // Dimensiones del tablero
    state->num_rows = 18;
    state->board = malloc(state->num_rows * sizeof(char*));
    if (!state->board) {
        free(state);
        return NULL;
    }

    // Tablero predeterminado
    char* default_board[] = {
        "####################",
        "#                  #",
        "# d>D    *         #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "#                  #",
        "####################"
    };

    // Copiar cada fila al tablero
    for (int i = 0; i < state->num_rows; i++) {
        state->board[i] = strdup(default_board[i]);
        if (!state->board[i]) {
            for (int j = 0; j < i; j++) free(state->board[j]);
            free(state->board);
            free(state);
            return NULL;
        }
    }

    // Inicializar serpiente
    state->num_snakes = 1;
    state->snakes = malloc(sizeof(snake_t));
    if (!state->snakes) {
        for (int i = 0; i < state->num_rows; i++) free(state->board[i]);
        free(state->board);
        free(state);
        return NULL;
    }

    state->snakes[0].tail_row = 2;
    state->snakes[0].tail_col = 2;
    state->snakes[0].head_row = 2;
    state->snakes[0].head_col = 4;
    state->snakes[0].live = true;

    return state;
}


/* Tarea 2 */
void free_state(game_state_t* state) {

   if (!state) return;

    // Liberar cada fila del tablero
    if (state->board) {
        for (unsigned int i = 0; i < state->num_rows; i++) {
            free(state->board[i]);
        }
        free(state->board);
    }

    // Liberar array de serpientes
    if (state->snakes) {
        free(state->snakes);
    }

    free(state);
}


/* Tarea 3 */
void print_board(game_state_t* state, FILE* fp) {
  if (!state || !fp) return;

    for (unsigned int i = 0; i < state->num_rows; i++) {
        fprintf(fp, "%s\n", state->board[i]);
    }
}


/**
 * Guarda el estado actual a un archivo. No modifica el objeto/struct state.
 * (ya implementada para que la utilicen)
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Tarea 4.1 */


/**
 * Funcion de ayuda que obtiene un caracter del tablero dado una fila y columna
 * (ya implementado para ustedes).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}


/**
 * Funcion de ayuda que actualiza un caracter del tablero dado una fila, columna y
 * un caracter.
 * (ya implementado para ustedes).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}


/**
 * Retorna true si la variable c es parte de la cola de una snake.
 * La cola de una snake consiste de los caracteres: "wasd"
 * Retorna false de lo contrario.
*/
static bool is_tail(char c) {
  return strchr("wasd", c) != NULL;
}


/**
 * Retorna true si la variable c es parte de la cabeza de una snake.
 * La cabeza de una snake consiste de los caracteres: "WASDx"
 * Retorna false de lo contrario.
*/
static bool is_head(char c) {
  return strchr("WASDx", c) != NULL;
}


/**
 * Retorna true si la variable c es parte de una snake.
 * Una snake consiste de los siguientes caracteres: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  return strchr("wasd^<v>WASDx", c) != NULL;
}


/**
 * Convierte un caracter del cuerpo de una snake ("^<v>")
 * al caracter que correspondiente de la cola de una
 * snake ("wasd").
*/
static char body_to_tail(char c) {
  switch (c) {
        case '^': return 'w';
        case '<': return 'a';
        case 'v': return 's';
        case '>': return 'd';
        default: return '?';
    }
}


/**
 * Convierte un caracter de la cabeza de una snake ("WASD")
 * al caracter correspondiente del cuerpo de una snake
 * ("^<v>").
*/
static char head_to_body(char c) {
  switch (c) {
        case 'W': return '^';
        case 'A': return '<';
        case 'S': return 'v';
        case 'D': return '>';
        default: return '?';
    }
}


/**
 * Retorna cur_row + 1 si la variable c es 'v', 's' o 'S'.
 * Retorna cur_row - 1 si la variable c es '^', 'w' o 'W'.
 * Retorna cur_row de lo contrario
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  if (strchr("vVsS", c)) return cur_row + 1;
  if (strchr("^Ww", c)) return cur_row - 1;
  return cur_row;
}


/**
 * Retorna cur_col + 1 si la variable c es '>' or 'd' or 'D'.
 * Retorna cur_col - 1 si la variable c es '<' or 'a' or 'A'.
 * Retorna cur_col de lo contrario
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  switch (c) {
        case '>':
        case 'd':
        case 'D':
            return cur_col + 1;
        case '<':
        case 'a':
        case 'A':
            // No permitir que la columna sea menor que 0
            return (cur_col == 0) ? 0 : cur_col - 1;
        default:
            return cur_col;
    }
}


/**
 * Tarea 4.2
 *
 * Funcion de ayuda para update_state. Retorna el caracter de la celda
 * en donde la snake se va a mover (en el siguiente paso).
 *
 * Esta funcion no deberia modificar nada de state.
*/
static char next_square(game_state_t* state, unsigned int snum) {
    snake_t snake = state->snakes[snum];
    char head = get_board_at(state, snake.head_row, snake.head_col);
    
    unsigned int next_row = get_next_row(snake.head_row, head);
    unsigned int next_col = get_next_col(snake.head_col, head);
    
    // Verificar bordes del tablero
    if (next_row >= state->num_rows || next_col >= strlen(state->board[next_row])) {
        return '#';
    }

    // Evitar colisiones laterales en Hydra
    if (is_head(head)) {
        for (unsigned i = 0; i < state->num_snakes; i++) {
            if (i != snum && state->snakes[i].live &&
                state->snakes[i].head_row == next_row &&
                state->snakes[i].head_col == next_col) {
                return '#'; // Tratar como colisión
            }
        }
    }
    return get_board_at(state, next_row, next_col);
}


/**
 * Tarea 4.3
 *
 * Funcion de ayuda para update_state. Actualiza la cabeza de la snake...
 *
 * ... en el tablero: agregar un caracter donde la snake se va a mover (¿que caracter?)
 *
 * ... en la estructura del snake: actualizar el row y col de la cabeza
 *
 * Nota: esta funcion ignora la comida, paredes, y cuerpos de otras snakes
 * cuando se mueve la cabeza.
*/
static void update_head(game_state_t* state, unsigned int snum) {
    snake_t* snake = &state->snakes[snum];
    char current_head = get_board_at(state, snake->head_row, snake->head_col);
    char new_head_dir = head_to_body(current_head);

    // Calcular nueva posición
    unsigned int new_row = get_next_row(snake->head_row, current_head);
    unsigned int new_col = get_next_col(snake->head_col, current_head);

    // Actualizar tablero
    set_board_at(state, snake->head_row, snake->head_col, new_head_dir);
    set_board_at(state, new_row, new_col, current_head);

    // Actualizar coordenadas de la cabeza
    snake->head_row = new_row;
    snake->head_col = new_col;
}


/**
 * Tarea 4.4
 *
 * Funcion de ayuda para update_state. Actualiza la cola de la snake...
 *
 * ... en el tablero: colocar un caracter blanco (spacio) donde se encuentra
 * la cola actualmente, y cambiar la nueva cola de un caracter de cuerpo (^<v>)
 * a un caracter de cola (wasd)
 *
 * ...en la estructura snake: actualizar el row y col de la cola
*/
static void update_tail(game_state_t* state, unsigned int snum) {
    snake_t* snake = &state->snakes[snum];
    char current_tail = get_board_at(state, snake->tail_row, snake->tail_col);
    
    // Encontrar la siguiente posición del cuerpo
    unsigned int next_row = get_next_row(snake->tail_row, current_tail);
    unsigned int next_col = get_next_col(snake->tail_col, current_tail);
    char next_body = get_board_at(state, next_row, next_col);
    
    // Convertir el cuerpo a tipo cola
    char new_tail_char = body_to_tail(next_body);
    
    // Actualizar el tablero
    set_board_at(state, snake->tail_row, snake->tail_col, ' ');  // Limpiar posición anterior
    set_board_at(state, next_row, next_col, new_tail_char);      // Nueva cola
    
    // Actualizar coordenadas de la cola en la estructura
    snake->tail_row = next_row;
    snake->tail_col = next_col;
}

/* Tarea 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
    // Primero actualizar todas las cabezas
    if (!state || !state->board) return;

    for (unsigned int i = 0; i < state->num_snakes; i++) {
        snake_t* snake = &state->snakes[i];
        if (!snake->live) continue;
        
        

        char next = next_square(state, i);
        if (next == '#' || is_snake(next)) {
            // Snake muere al chocar
            set_board_at(state, snake->head_row, snake->head_col, 'x');
            snake->live = false;
            continue;
        }

        update_head(state, i);
        if (next != '*') {
            update_tail(state, i);
        } else {
            // Comió comida, crece
            add_food(state);
        }
    }

    // Detección especial para Orochi (serpientes entrelazadas)
    for (unsigned i = 0; i < state->num_snakes; i++) {
        for (unsigned j = i+1; j < state->num_snakes; j++) {
            if (state->snakes[i].live && state->snakes[j].live &&
                state->snakes[i].head_row == state->snakes[j].head_row &&
                state->snakes[i].head_col == state->snakes[j].head_col) {
                // Solo una serpiente muere (la de menor índice)
                state->snakes[j].live = false;
                set_board_at(state, state->snakes[j].head_row, state->snakes[j].head_col, 'x');
            }
        }
    }
}

/* Tarea 5 */
game_state_t* load_board(char* filename) {
    if (!filename) return NULL;

    // 1. Abrir el archivo
    FILE* fp = fopen(filename, "r");
    if (!fp) return NULL;

    // 2. Crear estructura del estado
    game_state_t* state = malloc(sizeof(game_state_t));
    if (!state) {
        fclose(fp);
        return NULL;
    }

    // 3. Inicializar valores
    state->num_rows = 0;
    state->board = NULL;
    state->num_snakes = 0;
    state->snakes = NULL;

    char buffer[4096]; // Buffer para cada línea
    
    // 4. Leer el archivo línea por línea
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        // Eliminar salto de línea si existe
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len-1] == '\n') {
            buffer[len-1] = '\0';
            len--;
        }

        // Aumentar número de filas
        state->num_rows++;
        
        // Redimensionar array de filas
        state->board = realloc(state->board, state->num_rows * sizeof(char*));
        if (!state->board) {
            fclose(fp);
            free_state(state);
            return NULL;
        }
        
        // Copiar la línea al tablero
        state->board[state->num_rows-1] = strdup(buffer);
        if (!state->board[state->num_rows-1]) {
            fclose(fp);
            free_state(state);
            return NULL;
        }
    }

    // 5. Cerrar archivo y retornar estado
    fclose(fp);
    return state;
}


/**
 * Tarea 6.1
 *
 * Funcion de ayuda para initialize_snakes.
 * Dada una structura de snake con los datos de cola row y col ya colocados,
 * atravezar el tablero para encontrar el row y col de la cabeza de la snake,
 * y colocar esta informacion en la estructura de la snake correspondiente
 * dada por la variable (snum)
*/
static void find_head(game_state_t* state, unsigned int snum) {
    snake_t* snake = &state->snakes[snum];
    unsigned int row = snake->tail_row;
    unsigned int col = snake->tail_col;
    char current = get_board_at(state, row, col);

    while (!is_head(current)) {
        unsigned int next_row = get_next_row(row, current);
        unsigned int next_col = get_next_col(col, current);
        row = next_row;
        col = next_col;
        current = get_board_at(state, row, col);
    }

    snake->head_row = row;
    snake->head_col = col;
}

/* Tarea 6.2 */
game_state_t* initialize_snakes(game_state_t* state) {
   if (!state) return NULL;

    // Contar colas (serpientes)
    state->num_snakes = 0;
    for (unsigned int i = 0; i < state->num_rows; i++) {
        for (unsigned int j = 0; j < strlen(state->board[i]); j++) {
            if (is_tail(state->board[i][j])) {
                state->num_snakes++;
            }
        }
    }

    // Inicializar array de serpientes
    state->snakes = malloc(state->num_snakes * sizeof(snake_t));
    unsigned int idx = 0;
    for (unsigned int i = 0; i < state->num_rows; i++) {
        for (unsigned int j = 0; j < strlen(state->board[i]); j++) {
            if (is_tail(state->board[i][j])) {
                state->snakes[idx].tail_row = i;
                state->snakes[idx].tail_col = j;
                state->snakes[idx].live = true;
                find_head(state, idx);
                idx++;
            }
        }
    }

    return state;
}

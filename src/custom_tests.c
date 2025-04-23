#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asserts.h"

// Necesario por las funciones static en state.c
#include "state.c"

/* Vean el archivo asserts.c, para conocer algunas funciones de assert */

int greater_than_forty_two(int x) {
  return x > 42;
}

bool is_vowel(char c) {
  char* vowels = "aeiouAEIOU";
  for (int i = 0; i < strlen(vowels); i++) {
    if (c == vowels[i]) {
      return true;
    }
  }
  return false;
}

/**
 * Ejemplo 1: Retorna true si todos los tests pasan. false de lo contrario.
 *
 * La funcion greater_than_forty_two(int x) va a retornar true si x > 42. false de lo contrario.
 * Nota: Este test no cubre todo al 100%.
 */
bool test_greater_than_forty_two() {
  int testcase_1 = 42;
  bool output_1 = greater_than_forty_two(testcase_1);
  if (!assert_false("output_1", output_1)) {
    return false;
  }

  int testcase_2 = -42;
  bool output_2 = greater_than_forty_two(testcase_2);
  if (!assert_false("output_2", output_2)) {
    return false;
  }

  int testcase_3 = 4242;
  bool output_3 = greater_than_forty_two(testcase_3);
  if (!assert_true("output_3", output_3)) {
    return false;
  }

  return true;
}

/*
  Example 2: Returns true if all test cases pass. False otherwise.
    The function is_vowel(char c) will return true if c is a vowel (i.e. c is a,e,i,o,u)
    and returns false otherwise
    Note: This test is NOT comprehensive
*/
/**
 * Ejemplo 2: Retorna true si todos los tests pasan. false de lo contrario.
 *
 * La funcion is_vowel(char c) va a retornar true si c es una vocal (es decir, c es a,e,i,o,u)
 * y retorna false en el caso contrario
 * Nota: Este test no cubre todo al 100%.
 */
bool test_is_vowel() {
  char testcase_1 = 'a';
  bool output_1 = is_vowel(testcase_1);
  if (!assert_true("output_1", output_1)) {
    return false;
  }

  char testcase_2 = 'e';
  bool output_2 = is_vowel(testcase_2);
  if (!assert_true("output_2", output_2)) {
    return false;
  }

  char testcase_3 = 'i';
  bool output_3 = is_vowel(testcase_3);
  if (!assert_true("output_3", output_3)) {
    return false;
  }

  char testcase_4 = 'o';
  bool output_4 = is_vowel(testcase_4);
  if (!assert_true("output_4", output_4)) {
    return false;
  }

  char testcase_5 = 'u';
  bool output_5 = is_vowel(testcase_5);
  if (!assert_true("output_5", output_5)) {
    return false;
  }

  char testcase_6 = 'k';
  bool output_6 = is_vowel(testcase_6);
  if (!assert_false("output_6", output_6)) {
    return false;
  }

  return true;
}

/* Task 4.1 */

bool test_is_tail() {
    struct test_case {
        char input;
        bool expected;
    } tests[] = {
        {'w', true},
        {'a', true},
        {'s', true},
        {'d', true},
        {'W', false},
        {' ', false},
        {'*', false},
        {'^', false}
    };

    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        bool result = is_tail(tests[i].input);
        if (!assert_equals_bool("is_tail", tests[i].expected, result)) {
            return false;
        }
    }
    return true;
}

bool test_is_head() {
  struct test_case {
        char input;
        bool expected;
    } tests[] = {
        {'W', true},
        {'A', true},
        {'S', true},
        {'D', true},
        {'x', true},
        {'w', false},
        {'^', false},
        {'*', false}
    };

    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        bool result = is_head(tests[i].input);
        if (!assert_equals_bool("is_head", tests[i].expected, result)) {
            return false;
        }
    }
    return true;
}

bool test_is_snake() {
  struct test_case {
        char input;
        bool expected;
    } tests[] = {
        {'w', true},  // cola
        {'a', true},  // cola
        {'s', true},  // cola
        {'d', true},  // cola
        {'^', true},  // cuerpo
        {'<', true},  // cuerpo
        {'v', true},  // cuerpo
        {'>', true},  // cuerpo
        {'W', true},  // cabeza
        {'A', true},  // cabeza
        {'S', true},  // cabeza
        {'D', true},  // cabeza
        {'x', true},  // cabeza muerta
        {' ', false}, // espacio vacío
        {'#', false}, // pared
        {'*', false}, // comida
        {'@', false}  // carácter inválido
    };

    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        bool result = is_snake(tests[i].input);
        if (!assert_equals_bool("is_snake", tests[i].expected, result)) {
            printf("Failed case: '%c'\n", tests[i].input);
            return false;
        }
    }
    return true;
}

bool test_body_to_tail() {
  struct test_case {
        char input;
        char expected;
    } tests[] = {
        {'^', 'w'},
        {'<', 'a'},
        {'v', 's'},
        {'>', 'd'},
        {'W', '?'},  // Comportamiento indefinido según especificación
        {' ', '?'}
    };

    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        char result = body_to_tail(tests[i].input);
        if (!assert_equals_char("body_to_tail", tests[i].expected, result)) {
            return false;
        }
    }
    return true;
}

bool test_head_to_body() {
  struct test_case {
        char input;
        char expected;
    } tests[] = {
        {'W', '^'},  // arriba
        {'A', '<'},  // izquierda
        {'S', 'v'},  // abajo
        {'D', '>'},  // derecha
        {'w', '?'},  // comportamiento indefinido (no es cabeza)
        {'^', '?'},  // comportamiento indefinido (no es cabeza)
        {'x', '?'},  // comportamiento indefinido (cabeza muerta)
        {' ', '?'}   // comportamiento indefinido
    };

    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        char result = head_to_body(tests[i].input);
        if (!assert_equals_char("head_to_body", tests[i].expected, result)) {
            printf("Failed case: '%c'\n", tests[i].input);
            return false;
        }
    }
    return true;
}

bool test_get_next_row() {
  struct test_case {
        unsigned int cur_row;
        char input;
        unsigned int expected;
    } tests[] = {
        {5, 'v', 6},
        {5, 's', 6},
        {5, 'S', 6},
        {5, '^', 4},
        {5, 'w', 4},
        {5, 'W', 4},
        {5, 'a', 5},
        {5, '<', 5}
    };

    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        unsigned int result = get_next_row(tests[i].cur_row, tests[i].input);
        if (!assert_equals_unsigned_int("get_next_row", tests[i].expected, result)) {
            return false;
        }
    }
    return true;
}

bool test_get_next_col() {
  struct test_case {
        unsigned int cur_col;
        char input;
        unsigned int expected;
    } tests[] = {
        {5, '>', 6},  // derecha
        {5, 'd', 6},  // derecha (cola)
        {5, 'D', 6},  // derecha (cabeza)
        {5, '<', 4},  // izquierda
        {5, 'a', 4},  // izquierda (cola)
        {5, 'A', 4},  // izquierda (cabeza)
        {0, '<', 0},  // borde izquierdo - no puede moverse más allá
        {0, 'a', 0},  // borde izquierdo (cola)
        {0, 'A', 0},  // borde izquierdo (cabeza)
        {5, '^', 5},  // arriba (no afecta columna)
        {5, 'v', 5}   // abajo (no afecta columna)
    };

    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        unsigned int result = get_next_col(tests[i].cur_col, tests[i].input);
        if (!assert_equals_unsigned_int("get_next_col", tests[i].expected, result)) {
            printf("Failed case: cur_col=%u, input='%c'\n", 
                   tests[i].cur_col, tests[i].input);
            return false;
        }
    }
    return true;
}

bool test_customs() {
  if (!test_greater_than_forty_two()) {
    printf("%s\n", "test_greater_than_forty_two failed.");
    return false;
  }
  if (!test_is_vowel()) {
    printf("%s\n", "test_is_vowel failed.");
    return false;
  }
  if (!test_is_tail()) {
    printf("%s\n", "test_is_tail failed");
    return false;
  }
  if (!test_is_head()) {
    printf("%s\n", "test_is_head failed");
    return false;
  }
  if (!test_is_snake()) {
    printf("%s\n", "test_is_snake failed");
    return false;
  }
  if (!test_body_to_tail()) {
    printf("%s\n", "test_body_to_tail failed");
    return false;
  }
  if (!test_head_to_body()) {
    printf("%s\n", "test_head_to_body failed");
    return false;
  }
  if (!test_get_next_row()) {
    printf("%s\n", "test_get_next_row failed");
    return false;
  }
  if (!test_get_next_col()) {
    printf("%s\n", "test_get_next_col failed");
    return false;
  }
  return true;
}

int main(int argc, char* argv[]) {
  init_colors();

  if (!test_and_print("custom", test_customs)) {
    return 0;
  }

  return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#define BOARD_SIZE 10

// Состояния клеток: 0 - пусто, 1 - корабль, 2 - промах, 3 - попадание
int player_board[BOARD_SIZE][BOARD_SIZE] = { 0 };
int enemy_board[BOARD_SIZE][BOARD_SIZE] = { 0 };
int ships_left[5] = { 0, 4, 3, 2, 1 };

int is_inside(int row, int col) {
    return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE;
}

// Универсальная функция установки корабля на любое переданное поле
int place_ship(int board[BOARD_SIZE][BOARD_SIZE], int start_row, int start_col, int length, char direction) {
    int step_row = (direction == 's') - (direction == 'w');
    int step_col = (direction == 'd') - (direction == 'a');
    if (step_row == 0 && step_col == 0) step_col = 1;

    for (int i = 0; i < length; i++) {
        int current_row = start_row + i * step_row;
        int current_col = start_col + i * step_col;
        if (!is_inside(current_row, current_col)) return 0;

        for (int near_row = current_row - 1; near_row <= current_row + 1; near_row++) {
            for (int near_col = current_col - 1; near_col <= current_col + 1; near_col++) {
                if (is_inside(near_row, near_col) && board[near_row][near_col] != 0) {
                    return 0;
                }
            }
        }
    }

    for (int i = 0; i < length; i++) {
        board[start_row + i * step_row][start_col + i * step_col] = 1;
    }
    return 1;
}

// Случайная расстановка кораблей для бота
void place_random_ships(int board[BOARD_SIZE][BOARD_SIZE]) {
    int bot_ships[5] = { 0, 4, 3, 2, 1 };
    char dirs[4] = { 'w', 'a', 's', 'd' };

    for (int length = 4; length >= 1; length--) {
        while (bot_ships[length] > 0) {
            int r = rand() % BOARD_SIZE;
            int c = rand() % BOARD_SIZE;
            char d = dirs[rand() % 4];
            if (place_ship(board, r, c, length, d)) {
                bot_ships[length]--;
            }
        }
    }
}

// Отрисовка обоих полей
void draw_boards(int show_ships_left) {
    system("cls");
    printf("     YOUR BOARD               ENEMY BOARD\n");
    printf("   A B C D E F G H I J     A B C D E F G H I J\n");

    for (int row = 0; row < BOARD_SIZE; row++) {
        // Поле игрока 
        printf("%2d ", row + 1);
        for (int col = 0; col < BOARD_SIZE; col++) {
            int cell = player_board[row][col];
            char ch = '.';
            if (cell == 1) ch = '#';
            else if (cell == 2) ch = 'o';
            else if (cell == 3) ch = 'X';
            printf("%c ", ch);
        }

        // Поле противника 
        printf(" %2d ", row + 1);
        for (int col = 0; col < BOARD_SIZE; col++) {
            int cell = enemy_board[row][col];
            char ch = '.';
            if (cell == 2) ch = 'o';
            else if (cell == 3) ch = 'X';
            printf("%c ", ch);
        }
        printf("\n");
    }

    if (show_ships_left) {
        printf("\nShips left: [4]:%d [3]:%d [2]:%d [1]:%d\n\n",
            ships_left[4], ships_left[3], ships_left[2], ships_left[1]);
    }
    else {
        printf("\nLegend: [#] Your ship  [X] Hit  [o] Miss\n\n");
    }
}

// Обработка выстрела: 
int make_shot(int board[BOARD_SIZE][BOARD_SIZE], int row, int col) {
    if (board[row][col] == 2 || board[row][col] == 3) return -1;
    if (board[row][col] == 1) {
        board[row][col] = 3;
        return 1;
    }

    board[row][col] = 2;
    return 0;
}

// Проверка остались ли на поле целые палубы
int has_ships(int board[BOARD_SIZE][BOARD_SIZE]) {
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (board[r][c] == 1) return 1;
        }
    }
    return 0;
}

int main() {
    srand(time(NULL)); // Инициализация генератора случайных чисел
    char col_letter, direction;
    int row_number, length;

    // 1. Расстановка кораблей игрока
    while (ships_left[1] + ships_left[2] + ships_left[3] + ships_left[4] > 0) {
        draw_boards(1);
        printf("Place ship (e.g. A1 4 d) [w/a/s/d] or Q to quit: ");

        if (scanf(" %c", &col_letter) != 1 || toupper(col_letter) == 'Q') return 0;
        if (scanf("%d %d %c", &row_number, &length, &direction) != 3) {
            while (getchar() != '\n');
            continue;
        }

        int start_row = row_number - 1;
        int start_col = toupper(col_letter) - 'A';

        if (length >= 1 && length <= 4 && ships_left[length] > 0 &&
            place_ship(player_board, start_row, start_col, length, tolower(direction))) {
            ships_left[length]--;
        }
    }

    // 2 Расстановка кораблей противника
    place_random_ships(enemy_board);

    int last_status = -2; 

    // 3 Основной игровой цикл
    while (has_ships(player_board) && has_ships(enemy_board)) {
        draw_boards(0);

        if (last_status == 1) printf(">> HIT! Shoot again!\n");
        else if (last_status == 0) printf(">> MISS! Bot made its move.\n");
        else if (last_status == -1) printf(">> You already shot there! Try again.\n");

        printf("Enter target (e.g. A1) or Q to quit: ");
        if (scanf(" %c", &col_letter) != 1 || toupper(col_letter) == 'Q') return 0;
        if (scanf("%d", &row_number) != 1) {
            while (getchar() != '\n');
            continue;
        }

        int target_row = row_number - 1;
        int target_col = toupper(col_letter) - 'A';

        if (!is_inside(target_row, target_col)) continue;

        int result = make_shot(enemy_board, target_row, target_col);
        last_status = result;

        // Если игрок промахнулся, ходит бот
        if (result == 0) {
            int bot_res = 1;
            while (bot_res == 1 && has_ships(player_board)) {
                int br = rand() % BOARD_SIZE;
                int bc = rand() % BOARD_SIZE;
                bot_res = make_shot(player_board, br, bc);
                // Если бот попал в уже обстрелянную клетку
                if (bot_res == -1) bot_res = 1;
            }
        }
    }

    // 4. Финал игры
    draw_boards(0);
    if (has_ships(player_board)) {
        printf("=== YOU WIN ===\n");
    }
    else {
        printf("=== GAME OVER BOT WINS ===\n");
    }

    return 0;
}


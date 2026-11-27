#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>

#define SIZE 10

int my_board[SIZE][SIZE] = { 0 };
int bot_board[SIZE][SIZE] = { 0 };
int ships[5] = { 0, 4, 3, 2, 1 };

int inside(int r, int c) {
    return r >= 0 && r < SIZE && c >= 0 && c < SIZE;
}

int place(int board[SIZE][SIZE], int r, int c, int len, char dir) {
    int dr = (dir == 's') - (dir == 'w');
    int dc = (dir == 'd') - (dir == 'a');
    if (dr == 0 && dc == 0) dc = 1;

    for (int i = 0; i < len; i++) {
        int nr = r + i * dr;
        int nc = c + i * dc;
        if (!inside(nr, nc)) return 0;

        for (int x = nr - 1; x <= nr + 1; x++) {
            for (int y = nc - 1; y <= nc + 1; y++) {
                if (inside(x, y) && board[x][y] != 0) return 0;
            }
        }
    }

    for (int i = 0; i < len; i++) {
        board[r + i * dr][c + i * dc] = 1;
    }
    return 1;
}

void bot_place() {
    int left[5] = { 0, 4, 3, 2, 1 };
    char dirs[4] = { 'w', 'a', 's', 'd' };

    for (int len = 4; len >= 1; len--) {
        while (left[len] > 0) {
            int r = rand() % SIZE;
            int c = rand() % SIZE;
            char d = dirs[rand() % 4];
            if (place(bot_board, r, c, len, d)) left[len]--;
        }
    }
}

void draw(int show) {
    system("cls");
    printf("     YOUR BOARD               ENEMY BOARD\n");
    printf("   A B C D E F G H I J     A B C D E F G H I J\n");

    for (int r = 0; r < SIZE; r++) {
        printf("%2d ", r + 1);
        for (int c = 0; c < SIZE; c++) {
            int v = my_board[r][c];
            char ch = '.';
            if (v == 1) ch = '#';
            else if (v == 2) ch = 'o';
            else if (v == 3) ch = 'X';
            printf("%c ", ch);
        }

        printf(" %2d ", r + 1);
        for (int c = 0; c < SIZE; c++) {
            int v = bot_board[r][c];
            char ch = '.';
            if (v == 2) ch = 'o';
            else if (v == 3) ch = 'X';
            printf("%c ", ch);
        }
        printf("\n");
    }

    if (show) {
        printf("\nShips left: [4]:%d [3]:%d [2]:%d [1]:%d\n\n",
            ships[4], ships[3], ships[2], ships[1]);
    }
    else {
        printf("\nLegend: [#] Your ship  [X] Hit  [o] Miss\n\n");
    }
}

int shoot(int board[SIZE][SIZE], int r, int c) {
    if (board[r][c] == 2 || board[r][c] == 3) return -1;
    if (board[r][c] == 1) {
        board[r][c] = 3;
        return 1;
    }
    board[r][c] = 2;
    return 0;
}

int alive(int board[SIZE][SIZE]) {
    for (int r = 0; r < SIZE; r++)
        for (int c = 0; c < SIZE; c++)
            if (board[r][c] == 1) return 1;
    return 0;
}

void manual() {
    while (ships[1] + ships[2] + ships[3] + ships[4] > 0) {
        draw(1);

        int len;
        printf("Choose ship length (1-4) or Q to quit: ");
        char buf[10];
        if (scanf("%s", buf) != 1) continue;
        if (toupper(buf[0]) == 'Q') exit(0);

        len = atoi(buf);
        if (len < 1 || len > 4 || ships[len] == 0) {
            printf("Wrong length or no such ships left. Press Enter...\n");
            while (getchar() != '\n');
            getchar();
            continue;
        }

        printf("Enter coords (e.g. A1) and direction (w/a/s/d), or Q to cancel: ");
        char col;
        int row;
        char dir;

        if (scanf(" %c", &col) != 1) continue;
        if (toupper(col) == 'Q') continue;

        if (scanf("%d %c", &row, &dir) != 2) {
            while (getchar() != '\n');
            continue;
        }

        int r = row - 1;
        int c = toupper(col) - 'A';
        dir = tolower(dir);

        if (!inside(r, c) || (dir != 'w' && dir != 'a' && dir != 's' && dir != 'd')) {
            printf("Wrong coords or direction. Press Enter...\n");
            while (getchar() != '\n');
            getchar();
            continue;
        }

        if (place(my_board, r, c, len, dir)) {
            ships[len]--;
            printf("Ship placed! Press Enter...\n");
            while (getchar() != '\n');
            getchar();
        }
        else {
            printf("Cannot place here. Press Enter...\n");
            while (getchar() != '\n');
            getchar();
        }
    }
}

int main() {
    srand((unsigned int)time(NULL));

    manual();
    bot_place();

    int status = -2;

    while (alive(my_board) && alive(bot_board)) {
        draw(0);

        if (status == 1) printf(">> HIT! Shoot again!\n");
        else if (status == 0) printf(">> MISS! Bot moves.\n");
        else if (status == -1) printf(">> Already shot there! Try again.\n");

        printf("Enter target (e.g. A1) or Q to quit: ");
        char col;
        int row;
        if (scanf(" %c", &col) != 1 || toupper(col) == 'Q') return 0;
        if (scanf("%d", &row) != 1) {
            while (getchar() != '\n');
            continue;
        }

        int r = row - 1;
        int c = toupper(col) - 'A';

        if (!inside(r, c)) continue;

        int res = shoot(bot_board, r, c);
        status = res;

        if (res == 0) {
            int bot = 1;
            while (bot == 1 && alive(my_board)) {
                int br = rand() % SIZE;
                int bc = rand() % SIZE;
                int s = shoot(my_board, br, bc);
                if (s == -1) continue;
                bot = s;
            }
        }
    }

    draw(0);
    if (alive(my_board)) printf("=== YOU WIN ===\n");
    else printf("=== GAME OVER BOT WINS ===\n");

    return 0;
}
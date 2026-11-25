#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define SIZE 10

int my_grid[SIZE][SIZE] = { 0 };
int ships[5] = { 0, 4, 3, 2, 1 };

void draw_boards() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    printf("        YOUR BOARD                     ENEMY BOARD\n");
    printf("   A B C D E F G H I J           A B C D E F G H I J\n");

    for (int y = 0; y < SIZE; y++) {
        printf("%2d ", y + 1);
        for (int x = 0; x < SIZE; x++) {
            printf("%c ", my_grid[y][x] ? '#' : '.');
        }
        printf("       %2d ", y + 1);
        for (int x = 0; x < SIZE; x++) {
            printf(". ");
        }
        printf("\n");
    }
    printf("\nShips left: [4]:%d [3]:%d [2]:%d [1]:%d\n\n", ships[4], ships[3], ships[2], ships[1]);
}

int place_ship(int y, int x, int len, char dir) {
    int dy = (dir == 's') - (dir == 'w');
    int dx = (dir == 'd') - (dir == 'a');
    if (dy == 0 && dx == 0) dx = 1;

    for (int i = 0; i < len; i++) {
        int cy = y + i * dy, cx = x + i * dx;
        if (cy < 0 || cy >= SIZE || cx < 0 || cx >= SIZE) return 0;

        for (int ny = cy - 1; ny <= cy + 1; ny++) {
            for (int nx = cx - 1; nx <= cx + 1; nx++) {
                if (ny >= 0 && ny < SIZE && nx >= 0 && nx < SIZE && my_grid[ny][nx]) {
                    return 0;
                }
            }
        }
    }

    for (int i = 0; i < len; i++) {
        my_grid[y + i * dy][x + i * dx] = 1;
    }
    return 1;
}

int main() {
    char col_ch, dir = 'd';
    int row, len;

    while (ships[1] + ships[2] + ships[3] + ships[4] > 0) {
        draw_boards();
        printf("Place ship (e.g. A1 4 d) [dir: w/a/s/d] or Q to quit: ");

        if (scanf(" %c", &col_ch) != 1 || toupper(col_ch) == 'Q') return 0;
        if (scanf("%d %d %c", &row, &len, &dir) != 3) {
            while (getchar() != '\n');
            continue;
        }

        int x = toupper(col_ch) - 'A';
        int y = row - 1;
        dir = tolower(dir);

        if (len >= 1 && len <= 4 && ships[len] > 0 && place_ship(y, x, len, dir)) {
            ships[len]--;
        }
    }

    draw_boards();
    printf("All ships placed! Enter target (e.g. A1) or Q to quit:\n");
    while (scanf(" %c", &col_ch) == 1 && toupper(col_ch) != 'Q') {
        if (scanf("%d", &row) == 1) {
            int x = toupper(col_ch) - 'A';
            int y = row - 1;
            if (x >= 0 && x < SIZE && y >= 0 && y < SIZE) {
                printf("Shot at %c%d!\n", toupper(col_ch), row);
            }
        }
    }

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

int main() {
    srand(time(0));
    
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    
    const int COLS = w.ws_col;
    const int ROWS = w.ws_row;
    const int DELAY = 50000;
    
    if (COLS < 80 || ROWS < 24) {
        fprintf(stderr, "Terminal too small! Minimum 80x24 required.\n");
        return 1;
    }
    
    char *matrix[ROWS];
    for (int i = 0; i < ROWS; i++) {
        matrix[i] = malloc(COLS);
        memset(matrix[i], ' ', COLS);
    }
    
    int *column_start = malloc(COLS * sizeof(int));
    for (int i = 0; i < COLS; i++) {
        column_start[i] = -rand() % ROWS;
    }
    
    printf("\033[?25l");  // Hide cursor
    system("clear");
    
    while (1) {
        printf("\033[H");  // Move cursor to home position
        
        for (int j = 0; j < COLS; j++) {
            if (column_start[j] < ROWS) {
                column_start[j]++;
            } else {
                if (rand() % 20 == 0) {
                    column_start[j] = -rand() % (ROWS/2);
                }
            }
            
            int seg_length = rand() % 10 + 5;
            for (int i = 0; i < ROWS; i++) {
                if (i >= column_start[j] && i < column_start[j] + seg_length) {
                    matrix[i][j] = rand() % 2 ? (rand() % 10 + '0') : (rand() % 26 + 'A');
                } else if (i == column_start[j] + seg_length && rand() % 3 == 0) {
                    matrix[i][j] = ' ';
                }
            }
        }
        
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                if (matrix[i][j] != ' ') {
                    int bright = 0;
                    int trail = (i < ROWS - 1) ? 1 : 0;
                    
                    for (int k = 0; k < 3; k++) {
                        if (i - k >= 0 && matrix[i - k][j] != ' ') {
                            bright++;
                        }
                    }
                    
					if (bright == 1) {
					    printf("\033[38;2;200;200;255m%c\033[0m", matrix[i][j]); // Light blue
					} else if (bright == 2) {
					    printf("\033[38;2;150;150;255m%c\033[0m", matrix[i][j]); // Medium blue
					} else {
					    printf("\033[38;2;50;50;200m%c\033[0m", matrix[i][j]);   // Dark blue
					}
                    
                    if (trail && rand() % 8 == 0) {
                        matrix[i][j] = ' ';
                    }
                } else {
                    printf(" ");
                }
            }
        }
        
        fflush(stdout);
        usleep(DELAY);
    }
    
    printf("\033[?25h");  // Show cursor before exit
    return 0;
}

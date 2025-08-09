#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define COLS 80
#define ROWS 24
#define DELAY 80000
#define MIN_START_DELAY 5
#define MAX_START_DELAY 15

int main() {
    srand(time(0));
    
    char matrix[ROWS][COLS];
    int column_start[COLS];
    int column_length[COLS];
    
    
    for (int i = 0; i < COLS; i++) {
        column_start[i] = -rand() % (ROWS * 2);
        column_length[i] = rand() % 10 + 5;
    }
    
    printf("\033[?25l");  
    printf("\033[2J");    
    
    while (1) {
        printf("\033[H"); 
        
        
        for (int j = 0; j < COLS; j++) {
        
            if (column_start[j] > ROWS + column_length[j]) {
                if (rand() % 20 == 0) {
                    column_start[j] = -rand() % (ROWS * 2);
                    column_length[j] = rand() % 10 + 5;
                }
            }
            
           
            if (rand() % 3 != 0 || column_start[j] < 0) {
                column_start[j]++;
            }
           
            int clear_pos = column_start[j] - column_length[j] - 1;
            if (clear_pos >= 0 && clear_pos < ROWS) {
                matrix[clear_pos][j] = ' ';
            }
            
            int start = column_start[j];
            for (int i = start; i < start + column_length[j]; i++) {
                if (i >= 0 && i < ROWS) {
                    matrix[i][j] = (rand() % 10 > 3) 
                        ? (rand() % 10 + '0') 
                        : (rand() % 26 + 'A');
                }
            }
        }
        
        
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                if (matrix[i][j] != ' ') {
                    
                    int brightness = 0;
                    for (int k = 0; k < 3; k++) {
                        if (i - k >= 0 && matrix[i - k][j] != ' ') {
                            brightness++;
                        }
                    }
                    
                    if (brightness == 1) {
                        printf("\033[97m%c\033[0m", matrix[i][j]);  
                    } else if (brightness == 2) {
                        printf("\033[92m%c\033[0m", matrix[i][j]);  
                    } else {
                        printf("\033[32m%c\033[0m", matrix[i][j]);  
                    }
                    
                    
                    if (rand() % 8 == 0) {
                        matrix[i][j] = ' ';
                    }
                } else {
                    printf(" ");
                }
            }
            printf("\n");
        }
        
        fflush(stdout);
        usleep(DELAY);
    }
    
    printf("\033[?25h");
    return 0;
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define INPUT_SIZE 4
#define FIRST_ADD  7
#define SECOND_ADD 1443

int main() 
{

    printf("Enter your age : ");
    
    char input[INPUT_SIZE];
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\0")] = '\0';

    int age = atoi(input);
    age = age * FIRST_ADD;
    age = age * SECOND_ADD;
    
    printf("%d\n", age);
    
    return 0;
}

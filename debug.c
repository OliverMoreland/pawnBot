// C program to implement
// scanf
#include <stdio.h>
 
// Driver code
int main()
{
    char a, b;
   
      printf("Enter first number: ");
      scanf("%c\n", &a);
   
      printf("Enter second number: ");
      scanf("%c", &b);
   
      printf("A : %c \t B : %c" ,
            a , b);
   
    return 0;
}
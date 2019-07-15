#include <stdio.h>
#include "hotel.h"

int menu(void)
{
    int code, status;

    printf("\n%s%s\n", STARS, STARS);
    printf("Enter the number of the desired hotel:\n");
    printf("1) Fairfield Arms 2)Hotel Olympic");
    printf("3) Chertworthy Plaza 4)The Stockton");
    printf("5) quit\n");
    printf("\n%s%s\n", STARS, STARS);
    while((status = scanf("%d", &code)) != 1 || (code < 1 || code >5)) {
        if(status != 1)
            scanf("%*s");
        printf("Enter an integer from 1 to 5, please \n");
    }
    return code;
}

int getnights(void)
{
    int nights;

    printf("how many nights are needed");

    while(scanf("%d", &nights) != 1) {
        scanf("%*s");
        printf("Please enter an integer, such as 2");
    }
    return nights;
}

void showprices(double rate, int nights)
{
    int n;
    double total = 0.0;
    double factor = 1.0;
    
    for(n = 1;n <= nights; n++,factor *= DISCOUNT)
        total += rate * factor;

    printf("The total cost will be $%0.2f.\n", total);
}

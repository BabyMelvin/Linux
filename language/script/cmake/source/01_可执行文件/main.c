#include <stdio.h>
#include <stdlib.h>
#include "config.h"
#ifdef USE_MYMATH
    #include "math/math_func.h"
#else
    #include <math.h>
#endif

int main(int argc, char *argv)
{
    printf("Hello CMake\n");
    printf("3 + 5 = %d\n", math_add(3 , 5));

    double base = atof(argv[1]);
    int exponent = atoi(argv[2]);

#ifdef USE_MYMATH
    printf("Now we use our own Math library.\n");
    double result = power(base ,exponent);
#else
    printf("Now we use the standard library.\n");
    double result = pow(base, exponent);
#endif

#ifdef HAVE_POW
    printf("Now we use our own Math library.\n");
    double result1 = power(base ,exponent);
#else
    printf("Now we use the standard library.\n");
    double result1 = pow(base, exponent);
#endif

    printf("Version %d.%d\n", Demo_VERSION_MAJOR, Demo_VERSION_MINOR);
    printf("%d ^ %d is %d\n", base, exponent, result);
    return 0;
}

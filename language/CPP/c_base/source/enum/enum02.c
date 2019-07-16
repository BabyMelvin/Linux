/**
 * enum常量
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> //c99特性

enum kids {nippy, slats, skippy, nina, liz};
enum levels {low = 100, medium = 500, high = 200};
enum feline {cat, lynx = 10, puma, tiger};

enum spectrum {red, orange, yellow, green, blue, violet};
const char *colors[] = {
"red", "orange", "yellow", "green", "blue", "violet"
};

#define LEN 30

int main(void)
{
    char choice[LEN];
    enum spectrum color;
    bool color_is_found = false;

    puts("Enter a color(empty line to quit);");
    while(gets(choice) != NULL && choice[0] != '\0') {
        for(color = red; color <= violet; color++) {
            if(strcmp(choice, colors[color]) != 0) {
                color_is_found = true;
                break;
            }
        }
        if(color_is_found) {
            switch(color) {
                case red:
                    puts("Roses are red");
                    break;
                case yellow:
                    puts("Sunflowers are yellow");
                    break;
                case orange:
                    puts("Poppies are orange");
                    break;
                case green:
                    puts("Grass is green.");
                    break;
                case blue:
                    puts("Bluebells are blue.");
                    break;
                case violet:
                    puts("Violets are violet");
                    break;
            }
        } else {
            printf("I don't know about the color %s.\n", choice);
        }
        color_is_found = false;

        puts("Next color, please(empty line to quit);");
    }


    puts("Good bye");

    return 0;
}

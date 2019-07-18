/**
 * 定义和使用字段
 * */
#include <stdio.h>

/* 是否透明和是否可见*/
#define YES     1
#define NO      0

/* 边框线的样式 */
#define SOLD   0
#define DOTED  1
#define DASHED 2

/* 三原色 */ 
#define BLUE  4
#define GREEN 2
#define RED  1

/* 混合颜色 */
#define BLACK 0
#define YELLOW  (RED|GREEN)
#define MAGENTA (RED|BLUE)
#define CYAN    (GREEN|BLUE)
#define WHITE (RED|GREEN|BLUE)

const char *colors[]= {
    "black",
    "red",
    "green",
    "yellow",
    "blue",
    "magenta",
    "cyan",
    "white"
};

struct box_props {
    unsigned int opaque         : 1;
    unsigned int fill_color     : 3;
    unsigned int                : 4;
    unsigned int show_border    : 1;
    unsigned int border_color   : 3;
    unsigned int border_style   : 2;
    unsigned int                : 2;
};

void show_setting(const struct box_props *pb);

int main(void)
{
    struct box_props box = {YES, YELLOW, YES, GREEN, DASHED};
    printf("Original box setting:\n");

    show_setting(&box);

    box.opaque = NO;
    box.fill_color = WHITE;
    box.border_color = MAGENTA;
    box.border_style = SOLD;
    printf("\n Modified box setting:\n");
    show_setting(&box);

    return 0;
}

void show_setting(const struct box_props *pb)
{
    printf("Box is %s.\n", pb->opaque == YES? "opaque":"transparent");
    printf("The fill color is %s.\n", colors[pb->fill_color]);
    printf("Border %s.\n",
                pb->show_border == YES ? "shown":"not shown");

    printf("The border color is %s.\n", colors[pb->border_color]);
    printf("The border style is ");
    switch(pb->border_style) {
        case SOLD: printf("sold.\n"); break;
        case DOTED: printf("doted.\n"); break;
        case DASHED: printf("dashed.\n"); break;
        default: printf("unknown type.\n");
    }
}













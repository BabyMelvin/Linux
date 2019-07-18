/**
 * 位字段和位运算
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

/* 位运算中使用的常量 */
#define OPAQUE      0x1
#define FILL_BLUE   0x8
#define FILL_GREEN  0x4
#define FILL_RED    0x2
#define FILL_MASK   0xE

#define BORDER          0x100
#define BORDER_BLUE     0x800
#define BORDER_GREEN    0x400
#define BORDER_RED      0x200
#define BORDER_MASK     0xE00

#define B_SOLD          0
#define B_DOTTED        0x1000
#define B_DASHED        0X2000
#define STYLE_MASK      0x3000
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

//TODO 看成结构或者unsigned int 变量(可直接赋值)
union Views
{
    struct box_props st_view;
    unsigned int ui_view;
};

void show_setting(const struct box_props *pb);
void show_setting1(unsigned short);
char *itobs(int n, char *ps); /* 把short值以二进制字符串的形式显示*/

int main(void)
{
    //struct box_props box = {YES, YELLOW, YES, GREEN, DASHED};
    union Views box = {{YES, YELLOW, YES, GREEN, DASHED}};
    char bin_str[8 * sizeof(int) + 1];

    printf("Original box setting:\n");
    //show_setting(&box);
    show_setting(&box.st_view);
    printf("\nBox settings using unsigned int view.\n");
    printf("bit are %s\n", 
            itobs(box.ui_view, bin_str));


    box.ui_view &= ~FILL_MASK; /* 把代表填充色的位清0 */
    box.ui_view |= (FILL_BLUE | FILL_GREEN); /* 重置填充颜色 */
    box.ui_view ^= OPAQUE;   /* 转置指示是透明位 */
    box.ui_view |= BORDER_RED; /* 错误的方法 */
    box.ui_view &= ~STYLE_MASK; /* 清楚样式 */
    box.ui_view |= B_DOTTED; /* 把样式设置为点 */

    printf("\nModified box settings:\n");
    show_setting(&box.st_view);
    printf("\nBox settings using unsigned int view:\n");
    show_setting1(box.ui_view);
    printf("bits are %s.\n",
            itobs(box.ui_view, bin_str));
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

void show_setting1(unsigned short us)
{
    printf("\nbox is %s.\n",
            us & OPAQUE == OPAQUE ?"opaque":"transparent");
    printf("The fill color is %s.\n",
            colors[(us >> 1) & 07]);
    printf("Border %s.\n",
            us & BORDER == BORDER?"shown":"not shown");
    printf("The border style is ");
    switch(us & STYLE_MASK) {
        case B_SOLD: printf("solid.\n"); break;
        case B_DOTTED: printf("dotted.\n"); break;
        case B_DASHED: printf("dashed.\n"); break;
        default: printf("dashed.\n");
    }
    printf("The border color %s.\n",
            colors[(us >> 9) & 07]);
}

char *itobs(int n, char *ps)
{
    int i;
    static int size = 8 * sizeof(unsigned int);
    for (i = size -1; i >= 0; i--, n>>=1)
        ps[i] = (01 & n) + '0';
    ps[size] = '\0';

    return ps;
}

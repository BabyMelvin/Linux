/*简单预处理
 *  宏名字不允许有空格
 * */

#include <stdio.h>
#define TWO 2
#define OW "Consistency is the last refuge of the unimagina \
    tive, -Oscar Wilde" /*define 指令只允许单行*/

#define FOUR TWO * TWO
#define PX printf("X is %d.\n", x)
#define PMT "X is %d.\n"

//TODO 宏可用来初始化
#define LIMIT 20

#define WHAT "what"

#undef WHAT  /*取消WHAT定义*/
int main(void)
{
    int x = TWO;
    PX;

    printf(PMT, x);
    printf("%s\n", PMT);
    printf("%s\n", OW);
    printf("TWO: OW\n");

    const int LIM = 50;
    static int data[LIMIT]; //合法
    //static int data2[LIM];//非法
    
    const int LIM2 = 2 * LIMIT; //合法
    //const int LIM3 = 2 * LIM;   //非法
    return 0;
}

/*
 * 位字段
 * */

struct {
    unsigned int autfd:1;
    unsigned int bldfc:1;
    unsigned int undln:1;
    unsigned int itals:1;
} prnt;

//当总位数超过一个unsigned int大小，会使用下一个unsigned int进行存储
//不允许一个字段跨越两个unsigned int 之间的边界
//迫使下一个段与下一个整数对齐
struct {
    unsigned int field1:1;
    unsigned int       :2;
    unsigned int field2:1;     
    unsigned int       :0; 
    unsigned int field3:1;
} stuff;

int main(void)
{
    //TODO 1位的时候取值范围为 0或1
    prnt.itals = 0;
    prnt.undln = 1;
}


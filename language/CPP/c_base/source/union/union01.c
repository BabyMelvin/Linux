/**
 * 联合体使用:
 *  在某些结构中，该结构所存储的信息有赖于其中的一个成员
 * */

struct owner {
    char socsecurity[12];
};

struct leasecompany {
    char name[40];
    char headquarters[40];
};

union data {
    struct owner owncar;
    struct leasecompany leasecar;
};

struct car_data {
    char make[15];
    int status; /* 0 私有，1 租赁*/
    union data ownerinfo;
}

int main(void)
{
    struct car_data flits;
    scanf("输入0或1:%d", &flits.status);

    switch(flits.status) {
        case 0:
            strcpy(flits.ownerinfo.owncar.socsecurity, "owncar");
            break;
        case 1:
            strcpy(flits.leasecar.headquarters, "headquarters");
            break;
        default:
            break;
    }

    return 0;
}

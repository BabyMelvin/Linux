/**
 * cantool.c
 *
 * Socket CAN���߳���
 *
 * version: Ver1.0
 * Date   : 2015.11.16
 * Decrip : ԭʼ�汾
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef WIN32
#include <errno.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#else
#include <io.h>
#include <time.h>
#include "windows.h"
#endif

#ifndef WIN32
#include "can.h"
#include "raw.h" // for CAN_RAW_FILTER
#endif

#define _VERSION_   "Ver1.0"
#define _OWNER      "reille @ http://velep.com/"

#define true        1
#define false       0

typedef __u8        bool;
typedef __u8        byte;

typedef union u_canframe_data
{
    __u8            data[8];

    struct
    {
        __u32       dl;
        __u32       dh;
    } s;
} u_canframe_data_t;

static int  send_socket_fd = -1;
static int  recv_socket_fd = -1;

// -----------------------------------------------------------------------------

static void panic(const char *msg)
{
    printf(msg);
    exit(1);
}

void delay_ms(const __u32 ms)
{
    struct timespec tv;

    tv.tv_sec = ms / 1000;
    tv.tv_nsec = (long)(ms % 1000) * 1000000;

    nanosleep(&tv, NULL);
}

// �ж��ַ����Ƿ�Ϊ����
// h@ex: �Ƿ����ʮ����������
// @point: С�����Ƿ���Ч
bool str_isdigit(const char *s, const bool hex, const bool point)
{
    int i = 0;

    if (s == NULL || strlen(s) == 0)
    {
        return  false;
    }

    while (s[i] != '\0')
    {
        if (s[i] >='0' && s[i] <= '9' || (point && s[i] == '.') ||
            (hex && (s[i] >= 'a' && s[i] <= 'f')) || (hex && (s[i] >= 'A' && s[i] <= 'F')))
        {
            i++; 
        }
        else 
        {
            return  false;
        }
    }

    return  true;
}

//Function:1-byte ASC char converts to 1-byte unsigned char number
//In:
//	i_cAscNumber:the 1-byte ASC char to be converted ('0'~'9','A'~'F')
//Out:
//	no
//Return:
//	the converting 1-byte unsigned char number(0x0~0x0F)
//note:
//	'8' --> 0x08	'D' --> 0x0D
unsigned char Char_No1Hex(char i_cAscChar)
{
    unsigned char c;

    if (i_cAscChar>='A') {
        c=i_cAscChar-'A'+10;
    }else {
        c=i_cAscChar-48;
    }
    c%=16;

    return c;
}

//Function:2-byte ASC string converts to 1-byte unsigned char number
//In:
//	i_cpStr:pointer of the 2-byte ASC number string("00"~"FF")
//Out:
//	no
//Return:
//	the converting 1-byte unsigned char number
//note:
//	"28" --> 0x28	"7D" --> 0x7D	"AB" --> 0xAB
unsigned char String_Asc2TouChar(char *i_cpStr)
{
    unsigned char c;

    c = Char_No1Hex(i_cpStr[0])*0x10+Char_No1Hex(i_cpStr[1]);

    return c;

}

//Function:8-byte ASC string converts to 4-byte unsigned long number
//In:
//	i_cpStr:pointer of the 8-byte ASC number string("00000000"~"FFFFFFFF")
//Out:
//	no
//Return:
//	the 4-byte converting unsigned long number
//note:if ASC sring length >8,the get front 8 bytes
//	"00000072" --> 0x72 "789" --> 0x789 "12AC7850" --> 0x12AC7850 "12AC789950" --> 0x12AC7899
unsigned long String_Asc8TouLong(char *i_cpStr)
{
    int i,j;
    unsigned long lRet,value;
    unsigned char c;
    char tmp_cStr[9];

    for(j=0;j<9;j++) tmp_cStr[j]=0x30;
    j=(int)strlen(i_cpStr);
    if( j<8 ){
        for(i=0;i<j;i++)
            tmp_cStr[8-j+i]=i_cpStr[i];
    }
    else{
        for(i=0;i<8;i++)
            tmp_cStr[i]=i_cpStr[i];

    }
    value=1;
    lRet=0;
    for (i=7;i>=0;i--)
    {
        if (tmp_cStr[i]>=65)
            c=tmp_cStr[i]-65+10;
        else
            c=tmp_cStr[i]-48;
        lRet+=value*c;
        value=value*16;
    }

    return lRet;
}

// -----------------------------------------------------------------------------

typedef struct sys_error_frame
{
    __u32       id;
    char        *err_info;
} s_sys_error_frame_t;

static char g_errframe_info_buf[256] = {0};
static const s_sys_error_frame_t s_sys_error_frame[] = {
    { CAN_ERR_TX_TIMEOUT, "TX timeout (by netdevice driver)"    },
    { CAN_ERR_LOSTARB,    "lost arbitration    / data[0]"       },
    { CAN_ERR_CRTL,       "controller problems / data[1]"       },
    { CAN_ERR_PROT,       "protocol violations / data[2..3]"    },
    { CAN_ERR_TRX,        "transceiver status  / data[4]"       },
    { CAN_ERR_ACK,        "received no ACK on transmission"     },
    { CAN_ERR_BUSOFF,     "bus off"                             },
    { CAN_ERR_BUSERROR,   "bus error (may flood!)"              },
    { CAN_ERR_RESTARTED,  "controller restarted"                },
    
    { 0,                  NULL},
};

bool is_error_frame(const __u32 id)
{
    int i = 0;

    while (s_sys_error_frame[i].id && s_sys_error_frame[i].err_info != NULL)
    {
        if (id == s_sys_error_frame[i].id)
        {
            return  true;
        }

        i++;
    }
    return  false;
}

const char* get_errframe_info(const __u32 id, const char* data, const __u8 dlc)
{
    int l = 0;
    bool unspecified = false;
    char *p = g_errframe_info_buf;

    switch (id)
    {
    case CAN_ERR_TX_TIMEOUT:    sprintf(p, "���ͳ�ʱ");         break;
    case CAN_ERR_LOSTARB:       sprintf(p, "�����ٲô���");     break;
    case CAN_ERR_CRTL:          
        if (data && dlc >= 2)
        {
            switch (data[1])
            {
            case CAN_ERR_CRTL_RX_OVERFLOW:  l += sprintf(p + l, "���ջ������");    break;
            case CAN_ERR_CRTL_TX_OVERFLOW:  l += sprintf(p + l, "���ͻ������");    break;
            case CAN_ERR_CRTL_RX_WARNING:   l += sprintf(p + l, "���ձ���");        break;
            case CAN_ERR_CRTL_TX_WARNING:   l += sprintf(p + l, "���ͱ���");        break;
            case CAN_ERR_CRTL_RX_PASSIVE:   l += sprintf(p + l, "���ձ�������");    break;
            case CAN_ERR_CRTL_TX_PASSIVE:   l += sprintf(p + l, "���ͱ�������");    break;
            default:  unspecified = true;   break; /* unspecified */
            }
        }
        else
        {
            unspecified = true;
        }

        if (unspecified)
        {
            l += sprintf(p + l, "CAN����������");
        }
        break;

    case CAN_ERR_PROT:
        if (data && dlc >= 4)
        {
            if (data[2])
            {
                switch (data[2])
                {
                case CAN_ERR_PROT_BIT:      l += sprintf(p + l, "λ����");           break;
                case CAN_ERR_PROT_FORM:     l += sprintf(p + l, "֡��ʽ����");       break;
                case CAN_ERR_PROT_STUFF:    l += sprintf(p + l, "λ������");       break;
                case CAN_ERR_PROT_BIT0:     l += sprintf(p + l, "(����վ)���ܷ���ռ��λ(dominant bit)"); break;
                case CAN_ERR_PROT_BIT1:     l += sprintf(p + l, "(����վ)���ܷ��Ϳ���λ(recessive bit)"); break;
                case CAN_ERR_PROT_OVERLOAD: l += sprintf(p + l, "���߳�����");       break;
                case CAN_ERR_PROT_ACTIVE:   l += sprintf(p + l, "��������");         break;
                case CAN_ERR_PROT_TX:       l += sprintf(p + l, "�������");         break;
                default: unspecified = true;   break; /* unspecified */
                }
            }
            else
            {
                unspecified = true;
            }

            if (unspecified && data[3])
            {
                switch (data[3])
                {
                //case CAN_ERR_PROT_LOC_SOF:  l += sprintf(p + l, "");         break;
                default: unspecified = true;   break; /* unspecified */
                }
            }
        }
        else
        {
            unspecified = true;
        }

        if (unspecified)
        {
            l += sprintf(p + l, "Э��Υ��");
        }
        break;

    case CAN_ERR_TRX:
        break;

    case CAN_ERR_ACK:       sprintf(p, "Ӧ�����");               break;
    case CAN_ERR_BUSOFF:    sprintf(p, "���߹ر�");               break;
    case CAN_ERR_BUSERROR:  sprintf(p, "���ߴ���(�������)");     break;
    case CAN_ERR_RESTARTED: sprintf(p, "CAN����������");          break;

    default: break;
    }

    return  (const char *)p;
}

static void usage(void)
{
    int i = 0;

    printf("\n");
    printf("SOCKET CAN���߳��� - %s Build %s, COPYRIGHT (C) 2015 %s\n", _VERSION_, __DATE__, _OWNER);
    printf("\n");

    printf("����:  \n");
    printf("    ��SOCKET CAN����߱�ȫ��CAN���ܣ�����CAN��׼֡/��չ֡�����뷢�͡�CAN���ߴ����жϡ����صȹ���\n");
    printf("    ���û���LINUX SOCKET����ʵ�ֵ�CAN�ӿڣ�������Ƕ��ʽLINUX�е�CAN���Գ���\n");
    printf("    ������ñ�׼LINUX�����в���ѡ����ʽ�������û�����\n");
    printf("\n");

    printf("�÷�: ./cantool [ѡ��]...\n\n");

    printf("ѡ��:  \n");
    printf("    -p, --port=CAN�ӿں�        ָ��CAN�ӿںţ���1��ʼ��     Ĭ��Ϊ 1����CAN1�ӿڣ�\n");
    printf("    -b, --baud=������           ָ��CANͨѶ�����ʣ���λKbps��Ĭ��Ϊ 250 Kbps\n");
    printf("                                ���ò����ʣ�5��10��20��40��50��80��100��125��200��250��400��500��666��800��1000\n");
    printf("\n");

    printf("    -i, --frame-id=֡ID         ָ��CAN����֡ID��Hex��ʽ���� Ĭ��Ϊ1801F456\n");
    printf("    -d, --data=����             ָ��CAN����֡���ݣ�          Ĭ��Ϊ��00 01 FF FF FF FF FF FF���ֽ����ݼ��Կո����\n");
    printf("    -f, --freq=���             ָ��CAN֡���ͼ������λms��  Ĭ��Ϊ250ms, ��СֵΪ1ms\n");
    printf("    -t, --times=����            ָ��CAN֡���ʹ�����          Ĭ��Ϊ0��\n");
    printf("    -s,                         ָ��CAN����֡Ϊ��׼֡��      Ĭ��Ϊ������չ֡\n");
    printf("    -I,                         ֡IDÿ����һ֡������         Ĭ�ϲ�����\n");
    printf("    -g,                         ��������ÿ����һ֡������     Ĭ�ϲ�����\n");
    printf("    -l,                         ��������ʱ���ػ��أ�         Ĭ�ϲ�����\n");
    printf("\n");

    printf("        --help                  ��ʾ�˰�����Ϣ���˳�\n");
    printf("\n");

    printf("ע�⣬����CAN֡ID��Ϊϵͳʹ��:\n");
    while (s_sys_error_frame[i].id && strlen(s_sys_error_frame[i].err_info))
    {
    printf("    0x%08X - %s\n", s_sys_error_frame[i].id, s_sys_error_frame[i].err_info);
    i++;
    }
    printf("\n");

    printf("    ʹ�� Ctrl^C ��ϼ�������������\n");
    printf("\n");
}

// ���ѡ���Ƿ���Ҫ�û�����ֵ
bool option_no_value(const char *option)
{
    int i = 0;
    const char *options[] = {"-s", "-I", "-g", "-l", ""};

    while (strlen(options[i]))
    {
        if (!strcmp(option, options[i]))
        {
            return  true;
        }

        i++;
    }

    return  false;
}

// -----------------------------------------------------------------------------

bool find_can(const int port)
{
    char buf[128] = {0};
    sprintf(buf, "/sys/class/net/can%d/can_bittiming/bitrate", port);
    return  ((access(buf, 0) == 0));
}

/*
 *	bitrate: 250000 or 125000
 */
void set_bitrate(const int port, const int bitrate)
{
#define TX_QUEUE_LEN		4096 // ʹ���㹻��ķ��ͻ���

#ifndef WIN32
	char	l_c8Command[128] = {0};

	sprintf(l_c8Command, "echo %d > /sys/class/net/can%d/can_bittiming/bitrate", bitrate, port);
	system(l_c8Command);

	// ����tx_queue_len
	memset(l_c8Command, 0, sizeof(l_c8Command));
	sprintf(l_c8Command, "echo %d > /sys/class/net/can%d/tx_queue_len", TX_QUEUE_LEN, port);
	system(l_c8Command);
#endif
}

void open_can(const int port, const int bitrate)
{
#ifndef WIN32
    char	l_c8Command[64] = {0};

    sprintf(l_c8Command, "ifconfig can%d up", port);
    system(l_c8Command);
#endif
}

void close_can(const int port)
{
#ifndef WIN32
    char	l_c8Command[64] = {0};

    sprintf(l_c8Command, "ifconfig can%d down", port);
    system(l_c8Command);
#endif
}

void close_socket(const int sockfd)
{
#ifndef WIN32
    if (sockfd != -1)
    {
        close(sockfd);
    }
#endif
}

// ��sock��Ȼ������˿�
// ���ؼ��� �׽��� �ļ�������
int socket_listen(const int port)
{
    int sockfd = -1;

#ifndef WIN32
    struct sockaddr_can _addr;
    struct ifreq _ifreq;
    char buf[256];
    int ret = 0;

     /* �����׽��֣�����Ϊԭʼ�׽��֣�ԭʼCANЭ�� */
    sockfd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
    if (sockfd < 0) 
    {
        sprintf(buf, "\n\t�� socket can%d ����\n\n", port + 1);
        panic(buf);
        return  -1;
    }

     /* �����Ƕ�CAN�ӿڽ��г�ʼ����������CAN�ӿ���������������ifconfig����ʱ��ʾ������ */
    sprintf(buf, "can%d", port);
    strcpy(_ifreq.ifr_name, buf);
    ret = ioctl(sockfd, SIOCGIFINDEX, &_ifreq);
    if (ret < 0) 
    {
        sprintf(buf, "\n\tƥ�� socket can%d ����\n\n", port + 1);
        panic(buf);
        return  -1;
    }

    /* ����CANЭ�� */
    _addr.can_family = AF_CAN;
    _addr.can_ifindex = _ifreq.ifr_ifindex;


    /* disable default receive filter on this RAW socket */
    /* This is obsolete as we do not read from the socket at all, but for */
    /* this reason we can remove the receive list in the Kernel to save a */
    /* little (really a very little!) CPU usage.                          */
    //	setsockopt(sockfd, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

    /* �������ɵ��׽�����CAN�׽��ֵ�ַ���а� */
    ret = bind(sockfd, (struct sockaddr *)&_addr, sizeof(_addr));
    if ( ret < 0) 
    {
        close_socket(sockfd);
        sprintf(buf, "\n\t�� socket can%d ����\n\n", port + 1);
        panic(buf);
        return  -1;
    }
#endif

    return  sockfd;
}

int set_can_filter(void)
{
/**
 * struct can_filter - CAN ID based filter in can_register().
 * @can_id:   relevant bits of CAN ID which are not masked out.
 * @can_mask: CAN mask (see description)
 *
 * Description:
 * A filter matches, when
 *
 *          <received_can_id> & mask == can_id & mask
 *
 */
    const int n = 1;
    struct can_filter rfilter[n];

    // ���˹��򣺵�<received_can_id> & mask == can_id & maskʱ�����ձ��ģ�������˵�.
    // ����ͬʱ��Ӷ������˹���

    // ����������CAN֡��CAN_RAW�׽����ϲ������κ�CAN֡
    rfilter[0].can_id   = 0x00000000;
    rfilter[0].can_mask = CAN_EFF_MASK;
    (void)setsockopt(send_socket_fd, SOL_CAN_RAW, CAN_RAW_FILTER, rfilter, n * sizeof(struct can_filter));

    // ����������CAN֡��CAN_RAW�׽����Ͻ��ý��չ��˹���
    //(void)setsockopt(recv_socket_fd, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);

    return  0;
}

int set_can_loopback(const int sockfd, const bool lp)
{
    // ��Ĭ������£����ػػ������ǿ����ģ�����ʹ������ķ����رջػ�/�������ܣ�
    int loopback = lp;  // 0��ʾ�ر�, 1��ʾ����(Ĭ��) 
    (void)setsockopt(sockfd, SOL_CAN_RAW, CAN_RAW_LOOPBACK, &loopback, sizeof(loopback));

#if 0
    // �ڱ��ػػ����ܿ���������£����еķ���֡���ᱻ�ػ�����CAN���߽ӿڶ�Ӧ���׽����ϡ�
    // Ĭ������£�����CAN���ĵ��׽��ֲ�������Լ����͵ı��ģ���˷����׽����ϵĻػ������ǹرյġ�
    // ��������Ҫ��ʱ��ı���һĬ����Ϊ�� 
    int ro = 1; // 0��ʾ�ر�(Ĭ��), 1��ʾ����
    (void)setsockopt(sockfd, SOL_CAN_RAW, CAN_RAW_RECV_OWN_MSGS, &ro, sizeof(ro)); 
#endif

    return  0;
}

int socket_connect(const int port)
{
    return  socket_listen(port);
}

void disconnect(int *sockfd)
{
    if (!sockfd || *sockfd == -1)
    {
        return ;
    }

    close_socket(*sockfd);
    *sockfd = -1;
}

int send_frame(const int sockfd, const byte* data, const int count)
{
#ifndef WIN32
    int ret = write(sockfd, (const char*)data, count);
    if (ret > 0)
    {
        tcdrain(sockfd);//������������Ϊ���粻֧��tcdrain
    }
    return  ret;
#endif
}

int recv_frame(const int sockfd, byte* buf, const int count, const int timeout_ms)
{
    struct timeval tv_timeout;
    tv_timeout.tv_sec  = timeout_ms  / 1000;
    tv_timeout.tv_usec = (timeout_ms % 1000) * 1000;
    fd_set fs_read;
    
    FD_ZERO(&fs_read);
    FD_SET(sockfd, &fs_read);	//if fd == -1, FD_SET will block here

    int ret = select((int)sockfd + 1, &fs_read, NULL, NULL, &tv_timeout);
    if (ret == 0) // recv timeout
    {
        return  0;
    }
    if (ret < 0) // select error
    {
        return  ret;
    }

#ifndef WIN32
    ret = read(sockfd, (char*)buf, count);
#else
    ret = recv(sockfd, (char*)buf, count, 0);
#endif

    if (ret <= 0)
    {
        return  -1;
    }
    return  ret;
}

// -----------------------------------------------------------------------------

static int   *pframeno = NULL;
static int   port = 0;
static int   bitrate = 250 * 1000;
static __u32 send_frame_id  = 0x1801F456;
static __u32 send_frame_freq_ms = 250;
static int   send_frame_times   = 0;
static __u8  send_frame_data[8] = {0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static __u8  dlc = 8;
static bool  extended_frame         = true;
static bool  send_frame_id_inc_en   = false;// ֡IDÿ����һ֡����
static bool  send_frame_data_inc_en = false;// ��������ÿ����һ֡����
static bool  lp = false;// ���ػ���ʹ�ܿ���
static bool  run = true;


// return 0: normal; return 1: ��ֹ��������
int parse_options(int argc, char **argv)
{
    int  i = 0, j = 0, m = 0, n = 0;
    int  v = 0;
    char *p = NULL;
    bool long_option = false;
    bool ok = true;


    for (i = 1; i < argc; i++)
    {
        if (!argv[i])
        {
            continue;
        }

        long_option = false;
        p = NULL;

        //printf("--- i = %d, argc = %d, argv[%d]: %s ---\n", i, argc, i, argv[i]);

        if (strstr(argv[i], "--")) // ��ѡ��
        {
            long_option = true;
            p = strstr(argv[i], "=");
            if (p == NULL)
            {
                return  -1;
            }

            p += 1;
        }
        else if (strstr(argv[i], "-")) // ��ѡ��
        {
            if (option_no_value(argv[i]))
            {
                p = argv[i];
            } 
            else
            {
                if (argc <= (i + 1))
                {
                    return  -1;
                }

                p = argv[i + 1];
            }
        }
        else
        {
            return  -1;
        }


        ok = false;
        if (!strcmp(argv[i], "--help"))
        {
            return  1;
        }
        else if (!strcmp(argv[i], "-p") || strstr(argv[i], "--port") != NULL)
        {
            v = atoi(p);
            if (v >= 1 && v < 256)
            {
                port = v - 1;
                ok = true;
            }
        }
        else if (!strcmp(argv[i], "-b") || strstr(argv[i], "--baud") != NULL)
        {
            v = atoi(p);
            if (v == 5 || v == 10 || v == 20 || v == 40 || v == 50 || v == 80 || v == 100 || v == 125 || 
                v == 200 || v == 250 || v == 400 || v == 500 || v == 666 || v == 800 || v == 1000)
            {
                bitrate = v * 1000;
                ok = true;
            }
        }
        else if (!strcmp(argv[i], "-i") || strstr(argv[i], "--frame-id") != NULL)
        {
            send_frame_id = (__u32)String_Asc8TouLong(p);
            if ((send_frame_id & CAN_EFF_MASK))
            {
                return  -1;
            }
            send_frame_id &= CAN_EFF_MASK;
            if (is_error_frame(send_frame_id))
            {
                return  -1;
            }
            ok = true;
        }
        else if (!strcmp(argv[i], "-d") || strstr(argv[i], "--data") != NULL)
        {
            dlc = 0;

            if (long_option)
            {
                if (!str_isdigit(p, true, false))
                {
                    return  -1;
                }

                v = atoi(p);
                if (v < 0 || v > 256)
                {
                    return  -1;
                }

                send_frame_data[dlc] = (__u8)String_Asc2TouChar(p);
                dlc++;
            }

            for (j = i + 1; j < argc && dlc < 8; j++)
            {
                //printf("111 i = %d, j = %d, dlc = %d, argc = %d, argv[%d]: %s \n", i, j, dlc, argc, j, argv[j]);

                if (strstr(argv[j], "-") || strstr(argv[j], "--")) 
                {
                    j--;
                    break;
                }
                if (!str_isdigit(argv[j], true, false))
                {
                    return  -1;
                }

                v = atoi(argv[j]);
                if (v < 0 || v > 256)
                {
                    return  -1;
                }
                send_frame_data[dlc] = (__u8)String_Asc2TouChar(argv[j]);
                dlc++;
            }

            i = j;
            ok = true;

            if (i >= argc)
            {
                break;
            }

            //printf("222 i = %d, j = %d, dlc = %d, argc = %d, argv[%d]: %s \n", i, j, dlc, argc, j, argv[j]);

            continue;
        }
        else if (!strcmp(argv[i], "-f") || strstr(argv[i], "--freq") != NULL)
        {
            v = atoi(p);
            if (v >= 1)
            {
                send_frame_freq_ms = v;
                ok = true;
            }
        }
        else if (!strcmp(argv[i], "-t") || strstr(argv[i], "--times") != NULL)
        {
            v = atoi(p);
            if (v >= 0)
            {
                send_frame_times = v;
                ok = true;
            }
        }
        else if (!strcmp(argv[i], "-s"))
        {
            extended_frame = false;
            ok = true;
        }
        else if (!strcmp(argv[i], "-I"))
        {
            send_frame_id_inc_en = true;
            ok = true;
        }
        else if (!strcmp(argv[i], "-g"))
        {
            send_frame_data_inc_en = true;
            ok = true;
        }
        else if (!strcmp(argv[i], "-l"))
        {
            lp = true;
            ok = true;
        }



        if (!ok) // �û������˲��ܽ�����ѡ������
        {
            return  -1;
        }

        if (!long_option && !option_no_value(argv[i]))
        {
            i++;
        }
    }

    return  0;
}

void printf_head(void)
{
    printf("\n");
    printf("SOCKET CAN���߳���COPYRIGHT (C) 2015 %s \n", _OWNER);
    printf("CAN�ӿں� ��CAN%d        ͨѶ���� ��%d Kbps\n\n", port + 1, bitrate / 1000);


    // head
//            CAN1 ==> |  000001  |  18:09:00.356  |  ���ͳɹ�  | 0x1801F456 |  ��չ֡  |  8  |  00 01 FF FF FF FF FF FF
    printf("+----------+----------+----------------+------------+------------+----------+-----+-------------------------+\n");
    printf("|   �ӿ�   |   ���   |  ֡���ʱ��ms  |    ����    |    ֡ID    |  ֡��ʽ  | DLC |  ����                   |\n");
    printf("+----------+----------+----------------+------------+------------+----------+-----+-------------------------+\n");
}

// ÿ����һ�α�������֡����Լ�1
void printf_frame(const __u32 frame_id, const char *data, const __u8 len, 
    const bool extended, 
    const bool ok_flag, 
    const bool sendflag)
{
    int i = 0, l = 0;
    char buf[128] = {0};
    char timestamp[128] = {0};
    struct timeval tv;
    __u32 no = *pframeno;

    l = 0;
    for (i = 0; data && i < len && i < 8; i++)
    {
        l += sprintf(buf + l, "%02X ", data[i]);
    }

    /* ʱ���ַ��� */
    l = 0;
    (void)gettimeofday(&tv, NULL);
    (void)strftime(timestamp, 128, "%X", localtime(&tv.tv_sec));
    l = strlen(timestamp);
    (void)sprintf(timestamp + l, ".%03ld", tv.tv_usec/1000);

    printf(   "  CAN%d %s | %7u  |  %s  |",
        port + 1,
        sendflag ? "==>" : "<==",
        no, 
        timestamp);

    if (1 && is_error_frame(frame_id))
    {
    printf(   "  %s  | 0x%08X |  %s  |  %u  |  %s x %s\n",
        "���ߴ���",
        frame_id, 
        "����֡", 
        len, 
        buf,
        get_errframe_info(frame_id, data, len));
    }
    else
    {
    printf(   "  %s  | 0x%08X |  %s  |  %u  |  %s\n",
        ok_flag ? (sendflag ? "���ͳɹ�" : "���ճɹ�") : (sendflag ? "����ʧ��" : "����ʧ��"),
        frame_id, 
        extended ? "��չ֡" : "��׼֡", 
        len, 
        buf);    
    }


    no++;
    *pframeno = no;
}

void sig_handler(int signo) 
{ 
#ifndef WIN32
    switch(signo)
    {
    case SIGINT:
        run = false;
        printf(".Exit\n");
        break;

    default: break;
    }
#endif
}

void install_sig(void) 
{ 
#ifndef WIN32
    signal(SIGINT, sig_handler);
#endif
}

int main(int argc, char **argv)
{
    S_CanFrame sendframe, recvframe;
    byte *psendframe = (byte *)&sendframe;
    byte *precvframe = (byte *)&recvframe;
    u_canframe_data_t *psend_data = (u_canframe_data_t *)sendframe.data;
    const int can_frame_len = sizeof(S_CanFrame); 

    pid_t pid = -1;
    int   status;

    int  ret = 0;
    char buf[128] = {0};
    bool carry_bit = false;// ��λ��־

    int segment_id;//id for shared memo


    if (parse_options(argc, argv))
    {
        usage();    return  0;
    }

    if (!find_can(port))
    {
        sprintf(buf, "\n\t����CAN%d�豸������\n\n", port + 1);
        panic(buf);
        return  -1;
    }

    close_can(port);// �����ȹر�CAN�����ܳɹ�����CAN������
    set_bitrate(port, bitrate);// ����CAN֮ǰ����Ҫ���ò�����
    open_can(port, bitrate);

    send_socket_fd = socket_connect(port);
    recv_socket_fd = socket_connect(port);
    //printf("send_socket_fd = %d, recv_socket_fd = %d\n", send_socket_fd, recv_socket_fd);
    if (send_socket_fd < 0 || send_socket_fd < 0)
    {
        disconnect(&send_socket_fd);
        disconnect(&recv_socket_fd);
        panic("\n\t��socket can����\n\n");
        return  -1;
    }
    set_can_filter();
    set_can_loopback(send_socket_fd, lp);

    printf_head();

    memset(&sendframe, 0x00, sizeof(sendframe));
    memset(&recvframe, 0x00, sizeof(recvframe));

    if (extended_frame) // ָ������֡���ͣ���չ֡���׼֡
    {
        sendframe.can_id = (send_frame_id & CAN_EFF_MASK) | CAN_EFF_FLAG;
    } 
    else
    {
        sendframe.can_id = (send_frame_id & CAN_SFF_MASK);
    }
    sendframe.can_dlc = dlc;
    memcpy(sendframe.data, send_frame_data, dlc);

    
    segment_id = shmget(IPC_PRIVATE, sizeof(int), S_IRUSR | S_IWUSR);// allocate memo
    pframeno = (int *)shmat(segment_id, NULL, 0);// attach the memo
    if (pframeno == NULL)
    {
        panic("\n\t���������ڴ�ʧ��\n\n");
        return  -1;
    }
    *pframeno = 1;

    run = true;

    pid = fork();
    if(pid == -1) 
    { 
        panic("\n\t��������ʧ��\n\n");
        return  -1;
    }
    else if(pid == 0) // �ӽ��̣����ڷ���CAN֡
    {
        while (run && (send_frame_times > 0))
        {
            ret = send_frame(send_socket_fd, (char *)&sendframe, sizeof(sendframe));
            printf_frame(sendframe.can_id & CAN_EFF_MASK, sendframe.data, sendframe.can_dlc, 
                ((sendframe.can_id & CAN_EFF_FLAG) ? true : false),
                ret > 0 ? true : false, 
                true);
            delay_ms(send_frame_freq_ms);

            if (send_frame_id_inc_en)
            {
                sendframe.can_id++;
                if (extended_frame)
                {
                    sendframe.can_id = (sendframe.can_id & CAN_EFF_MASK) | CAN_EFF_FLAG;
                } 
                else
                {
                    sendframe.can_id = (sendframe.can_id & CAN_SFF_MASK);
                }
            }

            if (send_frame_data_inc_en && dlc > 0)
            {
                if (dlc > 4 && psend_data->s.dl == ((__u32)0xFFFFFFFF))
                {
                    carry_bit = true;// ������λ
                }
                psend_data->s.dl++;

                if (dlc <= 4)
                {
                    if (psend_data->s.dl >= (1 << (dlc * 8)))
                    {
                        psend_data->s.dl = 0;
                    }
                }
                else if (dlc <= 8)
                {
                    if (carry_bit)
                    {
                        psend_data->s.dh++;
                        if (psend_data->s.dh >= (1 << ((dlc - 4) * 8)))
                        {
                            psend_data->s.dh = 0;
                        }

                        carry_bit = false;
                    }
                }
            }

            send_frame_times--;
        }

        exit(0);
    }
    else // �����̣�����CAN֡
    {
        install_sig();

        while (run)
        {
            memset(precvframe, 0x00, can_frame_len);
            ret = recv_frame(recv_socket_fd, precvframe, can_frame_len, 5 * 1000);
            if (ret > 0)
            {
                printf_frame(recvframe.can_id & CAN_EFF_MASK, recvframe.data, recvframe.can_dlc, 
                    ((recvframe.can_id & CAN_EFF_FLAG) ? true : false),
                    true, 
                    false);
            }
        }

        while(((pid = wait(&status)) == -1) && (errno == EINTR))
        {
            delay_ms(10);
        }
    }

    disconnect(&send_socket_fd);
    disconnect(&recv_socket_fd);

    shmdt(pframeno);// detach memo
    shmctl(segment_id, IPC_RMID, NULL);// remove

    return  0;
}

// -----------------------------------------------------------------------------
// cantool.c

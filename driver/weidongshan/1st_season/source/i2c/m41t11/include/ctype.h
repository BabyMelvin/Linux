/*
 *  NOTE! This ctype does not handle EOF like the standard C
 *  library is required to.
 * */
#ifndef __CTYPE_H__
#define __CTYPE_H__
#define _U  0x01    /* upper */
#define _L  0x02    /* lower */
#define _D  0x04    /* digit */
#define _C  0x08    /* cntrl */
#define _P  0x10    /* punct */
#define _S  0x20    /* white space(space/lf/tab) */
#define _X  0x40    /* hex digit */
#define _SP 0x80    /* hard space (0x20) */

unsigned char _ctype01[] = {0x00,             /* EOF */
    _C,_C,_C,_C,_C,_C,_C,_C,                /* 0-7 */
    _C,_C|_S,_C|_S,_C|_S,_C|_S,_C|_S,_C,_C, /* 8-15 */
    _C,_C,_C,_C,_C,_C,_C,_C,                /* 16-23 */
    _C,_C,_C,_C,_C,_C,_C,_C,                /* 24-31 */
    _S|_SP,_P,_P,_P,_P,_P,_P,_P,            /* 32-39 */
    _P,_P,_P,_P,_P,_P,_P,_P,                /* 40-47 */
    _D,_D,_D,_D,_D,_D,_D,_D,                /* 48-55 */
    _D,_D,_P,_P,_P,_P,_P,_P,                /* 56-63 */
    _P,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U|_X,_U,/* 64-71 */
    _U,_U,_U,_U,_U,_U,_U,_U,                /* 72-79 */
    _U,_U,_U,_U,_U,_U,_U,_U,                /* 80-87 */
    _U,_U,_U,_P,_P,_P,_P,_P,                /* 88-95 */
    _P,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L|_X,_L,/* 96-103 */
    _L,_L,_L,_L,_L,_L,_L,_L,                /* 104-111 */
    _L,_L,_L,_L,_L,_L,_L,_L,                /* 112-119 */
    _L,_L,_L,_P,_P,_P,_P,_C,                /* 120-127 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        /* 128-143 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        /* 144-159 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        /* 160-175 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        /* 176-191 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        /* 192-207 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        /* 208-223 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,        /* 224-239 */
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0         /* 240-255 */ 
};     

//extern unsigned char _ctype[];
//#define __ismask(x) (_ctype[(int)(unsigned char)(x)])

#define __ismask(x) (_ctype01[(int)(unsigned char)(x + 1)])

#define isalnum(c) ((__ismask(c) & (_U|_L|_D) !=0)
#define isalpha(c) ((__ismask(c)&(_U|_L)) !=0)
#define iscntrl(c)	((__ismask(c)&(_C)) != 0)
static inline int isdigit(int c)
{
	return '0' <= c && c <= '9';
}
#define isgraph(c)	((__ismask(c)&(_P|_U|_L|_D)) != 0)
#define islower(c)	((__ismask(c)&(_L)) != 0)
#define isprint(c)	((__ismask(c)&(_P|_U|_L|_D|_SP)) != 0)
#define ispunct(c)	((__ismask(c)&(_P)) != 0)
/* Note: isspace() must return false for %NUL-terminator */
#define isspace(c)	((__ismask(c)&(_S)) != 0)
#define isupper(c)	((__ismask(c)&(_U)) != 0)
#define isxdigit(c)	((__ismask(c)&(_D|_X)) != 0)

#define isascii(c) (((unsigned char)(c))<=0x7f)
#define toascii(c) (((unsigned char)(c))&0x7f)

static inline unsigned char __tolower(unsigned char c)
{
	if (isupper(c))
		c -= 'A'-'a';
	return c;
}

static inline unsigned char __toupper(unsigned char c)
{
	if (islower(c))
		c -= 'a'-'A';
	return c;
}

#define tolower(c) __tolower(c)
#define toupper(c) __toupper(c)

/*
 * Fast implementation of tolower() for internal usage. Do not use in your
 * code.
 */
static inline char _tolower(const char c)
{
	return c | 0x20;
}

/* Fast check for octal digit */
static inline int isodigit(const char c)
{
	return c >= '0' && c <= '7';
}

#endif /* __CTYPE_H__ */

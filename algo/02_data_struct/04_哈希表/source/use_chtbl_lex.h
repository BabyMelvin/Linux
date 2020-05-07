#ifndef __LEX_H__
#define __LEX_H__

#include "chtbl.h"

/*Define the toke types recongnized by the lexical analyzer*/
typedef enum Token_ {lexit, error, digit, other} Token;

Token lex(const char *istream, CHTbl *symtbl);
#endif /*__LEX_H__*/

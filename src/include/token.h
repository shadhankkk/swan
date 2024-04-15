#ifndef TOKEN_H
#define TOKEN_H

typedef struct TOKEN_STRUCT
{
  enum
  {
    TOKEN_ID, // 0
    TOKEN_EQUALS, // 1
    TOKEN_UNARY_OPERATOR, // 2
    TOKEN_BINARY_OPERATOR, // 3
    TOKEN_STRING, // 4
    TOKEN_LITERAL, // 5
    TOKEN_SEMI, // 6
    TOKEN_LPAREN, // 7
    TOKEN_RPAREN, // 8
    TOKEN_LSQAURE_BRACKET, // 9
    TOKEN_RSQAURE_BRACKET, // 10
    TOKEN_LBRACE, // 11
    TOKEN_RBRACE, // 12
    TOKEN_COMMA, // 13
    TOKEN_EOF, // 14
  } type;

  char* value;
} token_T;

token_T* init_token(int type, char* value);
#endif

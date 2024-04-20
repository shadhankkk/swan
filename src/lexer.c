#include "include/lexer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

char whitespaceCharacter = ' ';
char nullCharacter = '\0';

lexer_T* init_lexer(char* contents)
{
  lexer_T* lexer = calloc(1, sizeof(struct LEXER_STRUCT));
  lexer->contents = contents;
  lexer->currentIndex = 0;
  lexer->currentCharacter = contents[lexer->currentIndex];

  return lexer;
}

void lexer_advance(lexer_T* lexer)
{
  if(lexer->currentCharacter != nullCharacter && lexer->currentIndex < strlen(lexer->contents))
  {
    ++lexer->currentIndex;
    lexer->currentCharacter = lexer->contents[lexer->currentIndex];
  }  
}

void lexer_skip_whitespace(lexer_T* lexer)
{
  while(lexer->currentCharacter == whitespaceCharacter || lexer->currentCharacter == 10)
  {
    lexer_advance(lexer);
  } 
}

token_T* lexer_get_next_token(lexer_T* lexer)
{
  while(lexer->currentCharacter != nullCharacter && lexer->currentIndex < strlen(lexer->contents))
  {

    if(lexer->currentCharacter == whitespaceCharacter || lexer->currentCharacter == 10)
      lexer_skip_whitespace(lexer);

    if(lexer->currentCharacter == '&')
    {
      lexer_advance(lexer);
      if(lexer->currentCharacter != '&')
      {
        printf("Invalid character '&'\n");
        exit(1);
      }

      return lexer_advance_with_token(lexer, init_token(TOKEN_BINARY_OPERATOR, "&&"));
    }

    if(lexer->currentCharacter == '|')
    {
      lexer_advance(lexer);
      if(lexer->currentCharacter != '|')
      {
        printf("Invalid character '|'\n");
        exit(1);
      }

      return lexer_advance_with_token(lexer, init_token(TOKEN_BINARY_OPERATOR, "||"));
    }

    if(isalnum(lexer->currentCharacter)) {
      token_T* curr_token = lexer_collect_id(lexer);

      int i=0;
      int dotcount = 0;
      while(curr_token->value[i] != '\0')
      {
        if(!isdigit(curr_token->value[i]))
        {
          if(curr_token->value[i] == '.')
          {
            ++dotcount;
          } else
          {
            return curr_token;
          }

          if(dotcount > 1)
          {
            return curr_token;
          }
        }
        ++i;
      }

      return init_token(TOKEN_LITERAL, curr_token->value);
    }

    if(lexer->currentCharacter == '"')
      return lexer_collect_string(lexer);

    if(lexer->currentCharacter == '=')
    {
      char* res_str = lexer_get_current_char_as_string(lexer);
      lexer_advance(lexer);
      if(lexer->currentCharacter == '=')
      {
        lexer_advance(lexer);
        res_str = realloc(res_str, 2 * sizeof(char));
        res_str[0] = '=';
        res_str[1] = '=';
        return init_token(TOKEN_BINARY_OPERATOR, res_str);
      }
      return init_token(TOKEN_EQUALS, res_str);
    }

    switch(lexer->currentCharacter)
    {
      // case '=':
      //   return lexer_advance_with_token(lexer, init_token(TOKEN_EQUALS, lexer_get_current_char_as_string(lexer))); break; 
      case '!':
        return lexer_advance_with_token(lexer, init_token(TOKEN_UNARY_OPERATOR, lexer_get_current_char_as_string(lexer))); break;
      case '>':
        return lexer_advance_with_token(lexer, init_token(TOKEN_BINARY_OPERATOR, lexer_get_current_char_as_string(lexer))); break;
      case '<':
        return lexer_advance_with_token(lexer, init_token(TOKEN_BINARY_OPERATOR, lexer_get_current_char_as_string(lexer))); break;
      case '+':
        return lexer_advance_with_token(lexer, init_token(TOKEN_BINARY_OPERATOR, lexer_get_current_char_as_string(lexer))); break;
      case '-':
        return lexer_advance_with_token(lexer, init_token(TOKEN_BINARY_OPERATOR, lexer_get_current_char_as_string(lexer))); break;
      case '*':
        return lexer_advance_with_token(lexer, init_token(TOKEN_BINARY_OPERATOR, lexer_get_current_char_as_string(lexer))); break;
      case '/':
        return lexer_advance_with_token(lexer, init_token(TOKEN_BINARY_OPERATOR, lexer_get_current_char_as_string(lexer))); break;
      // case '==':
      //   return lexer_advance_with_token(lexer, init_token(TOKEN_BINARY_OPERATOR, lexer_get_current_char_as_string(lexer))); break;
      case ';':
        return lexer_advance_with_token(lexer, init_token(TOKEN_SEMI, lexer_get_current_char_as_string(lexer))); break; 
      case '(':
        return lexer_advance_with_token(lexer, init_token(TOKEN_LPAREN, lexer_get_current_char_as_string(lexer))); break; 
      case ')':
        return lexer_advance_with_token(lexer, init_token(TOKEN_RPAREN, lexer_get_current_char_as_string(lexer))); break; 
      case '[':
        return lexer_advance_with_token(lexer, init_token(TOKEN_LSQAURE_BRACKET, lexer_get_current_char_as_string(lexer))); break;
      case ']':
        return lexer_advance_with_token(lexer, init_token(TOKEN_RSQAURE_BRACKET, lexer_get_current_char_as_string(lexer))); break;
      case '{':
        return lexer_advance_with_token(lexer, init_token(TOKEN_LBRACE, lexer_get_current_char_as_string(lexer))); break; 
      case '}':
        return lexer_advance_with_token(lexer, init_token(TOKEN_RBRACE, lexer_get_current_char_as_string(lexer))); break; 
      case ',':
        return lexer_advance_with_token(lexer, init_token(TOKEN_COMMA, lexer_get_current_char_as_string(lexer))); break; 
    }
  }

  return init_token(TOKEN_EOF, "\0");
}

token_T* lexer_collect_string(lexer_T* lexer)
{
  lexer_advance(lexer);

  char* value = calloc(1, sizeof(char));
  value[0] = nullCharacter;

  char endQuoteCharacter = '"';
  while(lexer->currentCharacter != endQuoteCharacter)
  {
    char* s = lexer_get_current_char_as_string(lexer);
    value = realloc(value, (strlen(value) + strlen(s) + 1) * sizeof(char));
    strcat(value, s);
  
    lexer_advance(lexer);
  }
  
  lexer_advance(lexer);

  return init_token(TOKEN_STRING, value);
}

token_T* lexer_collect_id(lexer_T* lexer)
{
  char* value = calloc(1, sizeof(char));
  value[0] = nullCharacter;

  char endQuoteCharacter = '"';
  while(isalnum(lexer->currentCharacter) || lexer->currentCharacter == '_' || lexer->currentCharacter == '.')
  {
    char* s = lexer_get_current_char_as_string(lexer);
    value = realloc(value, (strlen(value) + strlen(s) + 1) * sizeof(char));
    strcat(value, s);

    lexer_advance(lexer);
  }
  
  return init_token(TOKEN_ID, value);
}

token_T* lexer_advance_with_token(lexer_T* lexer, token_T* token)
{
  lexer_advance(lexer);

  return token;
}

char* lexer_get_current_char_as_string(lexer_T* lexer)
{
  char* str = calloc(2, sizeof(char));
  str[0] = lexer->currentCharacter;
  str[1] = nullCharacter;
  

  return str;
}

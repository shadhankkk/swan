#ifndef PARSER_H
#define PARSER_H
#include "./AST.h"
#include "./lexer.h"

typedef struct PARSER_STRUCT
{
  lexer_T* lexer;
  token_T* current_token;
  token_T* prev_token;
  AST_T** parsed_expr_stack;
  size_t parsed_expr_stack_size;
} parser_T;

parser_T* init_parser(lexer_T* lexer);

void parsed_expr_stack_push_back(parser_T* parser, AST_T* node);

AST_T* parsed_expr_stack_get_back(parser_T* parser);

void parsed_expr_stack_clear(parser_T* parser);

void parser_eat(parser_T* parser, int token_type);

AST_T* parser_parse(parser_T* parser);

AST_T* parser_parse_statement(parser_T* parser);

AST_T* parser_parse_statements(parser_T* parser);

AST_T* parser_parse_expression(parser_T* parser);

AST_T* parser_parse_lbrace(parser_T* parser);

AST_T* parser_parse_rbrace(parser_T* parser);

AST_T* parser_parse_factor(parser_T* parser);

AST_T* parser_parse_term(parser_T* parser);

AST_T* parser_parse_if_statement(parser_T* parser);

AST_T* parser_parse_while_loop(parser_T* parser);

AST_T* parser_parse_for_loop(parser_T* parser);

AST_T* parser_parse_function_call(parser_T* parser);

AST_T* parser_parse_variable_definition(parser_T* parser);

AST_T* parser_parse_function_definition(parser_T* parser);

AST_T* parser_parse_variable_assignment(parser_T* parser);

AST_T* parser_parse_array_access(parser_T* parser);

AST_T* parser_parse_variable(parser_T* parser);

AST_T* parser_parse_string(parser_T* parser);

AST_T* parser_parse_literal(parser_T* parser);

AST_T* parser_parse_array(parser_T* parser);

AST_T* parser_parse_unary_operator(parser_T* parser);

AST_T* parser_parse_binary_operator(parser_T* parser);

AST_T* parser_parse_return_statement(parser_T* parser);

AST_T* parser_parse_id(parser_T* parser);
#endif

#include "include/parser.h"
#include "include/queue.h"
#include "include/stack.h"
#include <stdio.h>
#include <string.h>

bool is_operator(char* s)
{
  return (strcmp(s, "+") == 0)
         || (strcmp(s,"-") == 0)
         || (strcmp(s, "/") == 0)
         || (strcmp(s, "+") == 0)
         || (strcmp(s, "&&") == 0)
         || (strcmp(s, "||") == 0)
         || (strcmp(s, ">") == 0)
         || (strcmp(s, "<") == 0)
         || (strcmp(s, "==") == 0);
}

int get_operator_precedence(char* s)
{
  return strcmp(s, "||") == 0
         ? 0
         : strcmp(s, "&&") == 0
         ? 1
         : strcmp(s, "==") == 0 || strcmp(s, ">") == 0 || strcmp(s, "<") == 0
         ? 2
         : strcmp(s, "+") == 0 || strcmp(s, "-") == 0
         ? 3
         : strcmp(s, "*") == 0 || strcmp(s, "/") == 0
         ? 4
         : -1;
}

parser_T* init_parser(lexer_T* lexer)
{
  parser_T* parser = calloc(1, sizeof(struct PARSER_STRUCT));
  parser->lexer = lexer;
  parser->current_token = lexer_get_next_token(lexer);
  parser->prev_token = parser->current_token;
  parser->parsed_expr_stack = (void*) 0;
  parser->parsed_expr_stack_size = 0;
  
  return parser;
}

void parsed_expr_stack_push_back(parser_T* parser, AST_T* node)
{

  ++parser->parsed_expr_stack_size;
  if(parser->parsed_expr_stack == (void*) 0)
  {
    parser->parsed_expr_stack = calloc(1, sizeof(struct AST_STRUCT*));
    parser->parsed_expr_stack[parser->parsed_expr_stack_size - 1] = node;
  }
  else
  {
    parser->parsed_expr_stack = realloc(
      parser->parsed_expr_stack,
      parser->parsed_expr_stack_size * sizeof(struct AST_STRUCT*)
      );
    parser->parsed_expr_stack[parser->parsed_expr_stack_size - 1] = node;
  }
}

AST_T* parsed_expr_stack_get_back(parser_T* parser)
{
  if(parser->parsed_expr_stack == (void*) 0)
  {
    printf("Invalid expression\n");
    exit(1);
  }
  else
  {
    return parser->parsed_expr_stack[parser->parsed_expr_stack_size - 1];
  }
}

bool parsed_expr_stack_is_empty(parser_T* parser)
{
  return parser->parsed_expr_stack_size == 0;
}

void parsed_expr_stack_clear(parser_T* parser)
{
  free(parser->parsed_expr_stack);
  parser->parsed_expr_stack = (void*) 0;
  parser->parsed_expr_stack_size = 0;
}

void parser_eat(parser_T* parser, int token_type)
{
  if(parser->current_token->type == token_type)
  {
    parser->prev_token = parser->current_token;
    parser->current_token = lexer_get_next_token(parser->lexer);
  }  
  else
  {
    printf(
      "Unexpected token '%s', with type %d, expected type %d\n",
      parser->current_token->value,
      parser->current_token->type,
      token_type
    );

    exit(1);
  }
}

AST_T* parser_parse(parser_T* parser)
{
  return parser_parse_statements(parser);
}

AST_T* parser_parse_statements(parser_T* parser)
{

  AST_T* compound = init_ast(AST_COMPOUND);

  compound->compound_value = calloc(1, sizeof(struct AST_STRUCT*));
  
  AST_T* ast_statement = parser_parse_statement(parser);
  compound->compound_value[0] = ast_statement;
  ++compound->compound_size;    
  
  while(parser->current_token->type == TOKEN_SEMI)
  {
    parser_eat(parser, TOKEN_SEMI);
    AST_T* ast_statement = parser_parse_statement(parser);
    
    if(ast_statement)
    {
      ++compound->compound_size;    
      compound->compound_value = realloc(compound->compound_value, compound->compound_size * sizeof(struct AST_STRUCT*));

      compound->compound_value[compound->compound_size - 1] = ast_statement;
    }

    parsed_expr_stack_clear(parser);
  }
  
  return compound;
}

AST_T* parser_parse_statement(parser_T* parser)
{
  if(parser->current_token->type == TOKEN_LPAREN)
  {
    parser_eat(parser, TOKEN_LPAREN);
    AST_T* res = parser_parse_statement(parser);
    parser_eat(parser, TOKEN_RPAREN);
    return res;
  }

  switch(parser->current_token->type)
  {
    case TOKEN_STRING: return parser_parse_string(parser); break;
    case TOKEN_LITERAL: return parser_parse_literal(parser); break;
    case TOKEN_ID: return parser_parse_id(parser); break;
    case TOKEN_UNARY_OPERATOR: return parser_parse_unary_operator(parser); break;
    case TOKEN_BINARY_OPERATOR: return parser_parse_binary_operator(parser); break;
  }

  return init_ast(AST_NOOP);
}

AST_T* parser_parse_expr(parser_T* parser)
{
  if(parser->current_token->type == TOKEN_LPAREN)
  {
    parser_eat(parser, TOKEN_LPAREN);
    AST_T* res = parser_parse_expr(parser);
    parser_eat(parser, TOKEN_RPAREN);
    parsed_expr_stack_push_back(parser, res);

    if(parser->current_token->type == TOKEN_BINARY_OPERATOR)
    {
      return parser_parse_expr(parser);
    }

    return res;
  }

  AST_T* res = init_ast(AST_NOOP);

  switch (parser->current_token->type)
  {
    case TOKEN_STRING: res = parser_parse_string(parser); break;
    case TOKEN_LITERAL: res = parser_parse_literal(parser); break;
    case TOKEN_ID: res = parser_parse_id(parser); break;
    case TOKEN_LSQAURE_BRACKET: res = parser_parse_array(parser); break;
    case TOKEN_UNARY_OPERATOR: res = parser_parse_unary_operator(parser); break;
    case TOKEN_BINARY_OPERATOR: res = parser_parse_binary_operator(parser); break;
  }

  parsed_expr_stack_push_back(parser, res);

  if(parser->current_token->type == TOKEN_BINARY_OPERATOR)
  {
    return parser_parse_expr(parser);
  }

  return res;
}

AST_T* parser_parse_factor(parser_T* parser)
{
  if(parser->current_token->type == TOKEN_LPAREN)
  {
    parser_eat(parser, TOKEN_LPAREN);
    AST_T* res = parser_parse_expr(parser);
    parser_eat(parser, TOKEN_RPAREN);

    return res;
  }
  AST_T* res = init_ast(AST_NOOP);

  switch (parser->current_token->type)
  {
    case TOKEN_STRING: res = parser_parse_string(parser); break;
    case TOKEN_LITERAL: res = parser_parse_literal(parser); break;
    case TOKEN_ID: res = parser_parse_id(parser); break;
    case TOKEN_LSQAURE_BRACKET: res = parser_parse_array(parser); break;
    case TOKEN_UNARY_OPERATOR: return parser_parse_unary_operator(parser); break;
  }

  return res;
}

AST_T* parser_parse_term(parser_T* parser)
{

}

AST_T* parser_parse_if_statement(parser_T* parser)
{
  parser_eat(parser, TOKEN_ID); // if
  AST_T* ast = init_ast(AST_IF_STATEMENT);

  parser_eat(parser, TOKEN_LPAREN);

  ast->if_statement_predicate = parser_parse_expr(parser);

  parser_eat(parser, TOKEN_RPAREN);
  parser_eat(parser, TOKEN_LBRACE);
  
  ast->if_statement_body = parser_parse_statements(parser);

  parser_eat(parser, TOKEN_RBRACE);

  if(strcmp(parser->current_token->value, "else") == 0)
  {
    parser_eat(parser, TOKEN_ID); // else

    if(strcmp(parser->current_token->value, "if") == 0)
    {
      ast->else_statement = parser_parse_if_statement(parser);
    }
    else
    {
      parser_eat(parser, TOKEN_LBRACE);

      ast->else_statement = parser_parse_statements(parser);

      parser_eat(parser, TOKEN_RBRACE);
    }
  }
  return ast;
}

AST_T* parser_parse_while_loop(parser_T* parser)
{

  AST_T* while_loop_ast = init_ast(AST_WHILE_LOOP);

  parser_eat(parser, TOKEN_ID); // while
  parser_eat(parser, TOKEN_LPAREN);

  while_loop_ast->while_loop_predicate = parser_parse_expr(parser);

  parser_eat(parser, TOKEN_RPAREN);

  parser_eat(parser, TOKEN_LBRACE);

  while_loop_ast->while_loop_body = parser_parse_statements(parser);

  parser_eat(parser, TOKEN_RBRACE);

  return while_loop_ast;
}

AST_T* parser_parse_for_loop(parser_T* parser)
{
  AST_T* for_loop_ast = init_ast(AST_FOR_LOOP);
  parser_eat(parser, TOKEN_ID); // for
  parser_eat(parser, TOKEN_LPAREN);
  for_loop_ast->for_loop_var_definition = parser_parse_expr(parser);
  parser_eat(parser, TOKEN_SEMI);
  for_loop_ast->for_loop_predicate = parser_parse_expr(parser);
  parser_eat(parser, TOKEN_SEMI);
  for_loop_ast->for_loop_assignment = parser_parse_expr(parser);
  parser_eat(parser, TOKEN_RPAREN);

  parser_eat(parser, TOKEN_LBRACE);

  for_loop_ast->for_loop_body = parser_parse_statements(parser);

  parser_eat(parser, TOKEN_RBRACE);

  return for_loop_ast;
}

AST_T* parser_parse_function_call(parser_T* parser)
{

  AST_T* function_call = init_ast(AST_FUNCTION_CALL);
  
  function_call->function_call_name = parser->prev_token->value;
  parser_eat(parser, TOKEN_LPAREN);
  
  if(parser->current_token->type == TOKEN_RPAREN)
  {
    function_call->function_call_arguments_size = 0;
    parser_eat(parser, TOKEN_RPAREN);
    return function_call;
  }

  function_call->function_call_arguments = calloc(1, sizeof(struct AST_STRUCT*));
  AST_T* ast_expr = parser_parse_expr(parser);
  function_call->function_call_arguments[0] = ast_expr;
  ++function_call->function_call_arguments_size;

  while(parser->current_token->type == TOKEN_COMMA)
  {
    parser_eat(parser, TOKEN_COMMA);

    AST_T* ast_expr = parser_parse_expr(parser);
    ++function_call->function_call_arguments_size;
    
    function_call->function_call_arguments = realloc(
      function_call->function_call_arguments,
      function_call->function_call_arguments_size * sizeof(struct AST_STRUCT*)
    );

    function_call->function_call_arguments[function_call->function_call_arguments_size - 1] = ast_expr;
  }

  parser_eat(parser, TOKEN_RPAREN);
  return function_call;  
}

AST_T* parser_parse_variable_definition(parser_T* parser)
{

  parser_eat(parser, TOKEN_ID); // var
  char* variable_definition_variable_name = parser->current_token->value;
  parser_eat(parser, TOKEN_ID); // var name
  parser_eat(parser, TOKEN_EQUALS);
  AST_T* variable_definition_value = parser_parse_expr(parser);
  AST_T* original_variable_definition_value = variable_definition_value;

  AST_T* variable_definition = init_ast(AST_VARIABLE_DEFINITION);
  variable_definition->variable_definition_variable_name = variable_definition_variable_name;
  variable_definition->variable_definition_value = variable_definition_value;
  
  return variable_definition;
}

AST_T* parser_parse_function_definition(parser_T* parser)
{
  AST_T* ast = init_ast(AST_FUNCTION_DEFINITION);
  parser_eat(parser, TOKEN_ID); // function
  
  char* function_name = parser->current_token->value;
  ast->function_definition_name = calloc(strlen(function_name) + 1, sizeof(char));
  strcpy(ast->function_definition_name, function_name);

  parser_eat(parser, TOKEN_ID); // function name
  
  parser_eat(parser, TOKEN_LPAREN);
  
  while(parser->current_token->type != TOKEN_RPAREN)
  {
    ++ast->function_definition_arguments_size;

    if(ast->function_definition_arguments == (void*) 0)
    {
      ast->function_definition_arguments = calloc(1, sizeof(char*));
      ast->function_definition_arguments[0] = parser->current_token->value;
      parser_eat(parser, TOKEN_ID);
    }
    else
    {
      ast->function_definition_arguments = realloc(
        ast->function_definition_arguments, 
        ast->function_definition_arguments_size * sizeof(char*)
      );

      ast->function_definition_arguments[ast->function_definition_arguments_size - 1] = parser->current_token->value;
      parser_eat(parser, TOKEN_ID);
    }
    if(parser->current_token->type != TOKEN_RPAREN)
      parser_eat(parser, TOKEN_COMMA); 
  }

  

  parser_eat(parser,TOKEN_RPAREN);

  parser_eat(parser, TOKEN_LBRACE);

  ast->function_definition_body = parser_parse_statements(parser);
  ++ast->function_definition_body->compound_size;
  ast->function_definition_body->compound_value = realloc(
    ast->function_definition_body->compound_value,
    ast->function_definition_body->compound_size * sizeof(struct AST_STRUCT*)
  );
  AST_T* end_of_function_ast = init_ast(AST_END_OF_FUNCTION);
  ast->function_definition_body->compound_value[ast->function_definition_body->compound_size - 1] = end_of_function_ast;

  parser_eat(parser, TOKEN_RBRACE);

  return ast;
}
 
AST_T* parser_parse_variable_assignment(parser_T* parser)
{
  char* variable_name = parser->prev_token->value;

  AST_T* ast_variable = init_ast(AST_VARIABLE);
  ast_variable->variable_name = variable_name;

  parser_eat(parser, TOKEN_EQUALS); // =

  AST_T* new_value = parser_parse_expr(parser);

  AST_T* assignment_ast = init_ast(AST_ASSIGNMENT);

  assignment_ast->assignment_variable = ast_variable;
  assignment_ast->new_assignment_value = new_value;
  
  return assignment_ast;
}

AST_T* parser_parse_array_access(parser_T* parser)
{
  AST_T* array_ast = parsed_expr_stack_get_back(parser);
  parser_eat(parser, TOKEN_LSQAURE_BRACKET);
  AST_T* index = parser_parse_expr(parser);
  parser_eat(parser, TOKEN_RSQAURE_BRACKET);

  if(parser->current_token->type == TOKEN_EQUALS)
  {
    parser_eat(parser, TOKEN_EQUALS);
    AST_T* res = init_ast(AST_ARRAY_ASSIGNMENT);
    res->array_assignment_index = index;
    res->new_array_assignment_value = parser_parse_expr(parser);
    res->array_assignment_array = array_ast;

    return res;
  }
  else
  {
    AST_T* res = init_ast(AST_ARRAY_ACCESS);
    res->array_access_index = index;
    res->array_access_array = array_ast;

    if(parser->current_token->type == TOKEN_LSQAURE_BRACKET)
    {
      parsed_expr_stack_push_back(parser, res);
      return parser_parse_array_access(parser);
    }

    // if(parser->current_token->type == TOKEN_BINARY_OPERATOR)
    // {
    //   parsed_expr_stack_push_back(parser, res);
    //   return parser_parse_expr(parser);
    // }

    return res;
  }

}

AST_T* parser_parse_variable(parser_T* parser)
{
  
  char* token_value = parser->current_token->value;
  parser_eat(parser, TOKEN_ID); // var name or function call name

  if(parser->current_token->type == TOKEN_LPAREN)
  {
    return parser_parse_function_call(parser);
  }
  else if(parser->current_token->type == TOKEN_EQUALS)
  {
    return parser_parse_variable_assignment(parser);
  }
  else if(parser->current_token->type == TOKEN_LSQAURE_BRACKET)
  {
    AST_T* ast_variable = init_ast(AST_VARIABLE);
    ast_variable->variable_name = token_value;
    parsed_expr_stack_push_back(parser, ast_variable);

    return parser_parse_array_access(parser);
  }
  else
  {
    AST_T* ast_variable = init_ast(AST_VARIABLE);
    ast_variable->variable_name = token_value;

    // if(parser->current_token->type == TOKEN_BINARY_OPERATOR)
    // {
    //   parsed_expr_stack_push_back(parser, ast_variable);
    //   return parser_parse_expr(parser);
    // }
  
    return ast_variable;
  }
}

AST_T* parser_parse_string(parser_T* parser)
{
  AST_T* ast_string = init_ast(AST_STRING);
  ast_string->string_value = parser->current_token->value;

  parser_eat(parser, TOKEN_STRING);

  // if(parser->current_token->type == TOKEN_BINARY_OPERATOR)
  // {
  //   parsed_expr_stack_push_back(parser, ast_string);
  //   return parser_parse_expr(parser);
  // }

  return ast_string;
}

AST_T* parser_parse_literal(parser_T* parser)
{
  AST_T* ast_literal = init_ast(AST_LITERAL);
  ast_literal->literal_value = atof(parser->current_token->value);

  parser_eat(parser, TOKEN_LITERAL);

  // if(parser->current_token->type == TOKEN_BINARY_OPERATOR)
  // {
  //   parsed_expr_stack_push_back(parser, ast_literal);
  //   return parser_parse_expr(parser);
  // }

  return ast_literal;
}

AST_T* parser_parse_array(parser_T* parser)
{
  AST_T* array_ast = init_ast(AST_ARRAY);
  parser_eat(parser, TOKEN_LSQAURE_BRACKET); // [
  if(strcmp(parser->current_token->value, "]") == 0)
  {
    parser_eat(parser, TOKEN_RSQAURE_BRACKET);
    return array_ast;
  }

  array_ast->array = calloc(1, sizeof(struct AST_STRUCT*));
  ++array_ast->array_size;
  array_ast->array[0] = parser_parse_expr(parser);

  while(parser->current_token->type == TOKEN_COMMA)
  {
    parser_eat(parser, TOKEN_COMMA);
    ++array_ast->array_size;
    array_ast->array = realloc(
      array_ast->array,
      array_ast->array_size * sizeof(struct AST_STRUCT*)
    );
    array_ast->array[array_ast->array_size - 1] = parser_parse_expr(parser);
  }

  parser_eat(parser, TOKEN_RSQAURE_BRACKET);

  return array_ast;
}

AST_T* parser_parse_unary_operator(parser_T* parser)
{
  AST_T* unary_op_ast = init_ast(AST_UNARY_OPERATOR);
  unary_op_ast->unary_operator = parser->current_token->value;
  parser_eat(parser, TOKEN_UNARY_OPERATOR); // unary operator
  unary_op_ast->unary_operand = parser_parse_expr(parser);

  return unary_op_ast;
}

AST_T* parser_parse_binary_operator(parser_T* parser)
{
  // if(strcmp(parser->current_token->value, "-") == 0 && (parsed_expr_stack_is_empty(parser) || parser->prev_token->type == TOKEN_BINARY_OPERATOR))
  // {
  //   printf("hola\n");
  //   AST_T* un_op_ast = init_ast(AST_UNARY_OPERATOR);
  //   un_op_ast->unary_operator = "-";
  //   parser_eat(parser, TOKEN_BINARY_OPERATOR);
  //   un_op_ast->unary_operand = parser_parse_expr(parser);

  //   return un_op_ast;
  // }
  
  // AST_T* bin_op_ast = init_ast(AST_BINARY_OPERATOR);

  // bin_op_ast->binary_operator = parser->current_token->value;
  
  // bin_op_ast->operand1 = parsed_expr_stack_get_back(parser);

  // parser_eat(parser, TOKEN_BINARY_OPERATOR); // binary operator
  
  // bin_op_ast->operand2 = parser_parse_expr(parser);

  // return bin_op_ast;

  if(parsed_expr_stack_is_empty(parser) && strcmp(parser->current_token->value, "-") == 0)
  {
    AST_T* un_op_ast = init_ast(AST_UNARY_OPERATOR);
    un_op_ast->unary_operator = "-";
    parser_eat(parser, TOKEN_BINARY_OPERATOR);
    un_op_ast->unary_operand = parser_parse_factor(parser);
    
    return un_op_ast;
  }

  queue_T* q = init_queue();
  stack_T* s = init_stack();
  enqueue(q, parsed_expr_stack_get_back(parser));

  while(parser->current_token->type == TOKEN_BINARY_OPERATOR)
  {

    AST_T* op_ast = init_ast(AST_BINARY_OPERATOR);
    op_ast->binary_operator = parser->current_token->value;

    parser_eat(parser, TOKEN_BINARY_OPERATOR); // eat binary
    
    while(!is_empty_stack(s) && get_operator_precedence(op_ast->binary_operator) <= get_operator_precedence(stack_get_back(s)->binary_operator))
    {
      enqueue(q, stack_get_back(s));
      stack_pop_back(s);
    }

    stack_push_back(s, op_ast);
    if(parser->current_token->type == TOKEN_BINARY_OPERATOR)
    {
      AST_T* unary_op_ast = init_ast(AST_UNARY_OPERATOR);
      unary_op_ast->unary_operator = parser->current_token->value;
      parser_eat(parser, TOKEN_BINARY_OPERATOR); // eat unary
      unary_op_ast->unary_operand = parser_parse_factor(parser);
      enqueue(q, unary_op_ast);
      continue;
    }
    enqueue(q, parser_parse_factor(parser));
  }

  while(!is_empty_stack(s))
  {
    enqueue(q, stack_get_back(s));
    stack_pop_back(s);
  }

  while(!is_empty_queue(q))
  {
    AST_T* qfront = dequeue(q);
    if(qfront->type == AST_BINARY_OPERATOR && qfront->operand1 == (void*) 0  && qfront->operand2 == (void*) 0)
    {
      qfront->operand2 = stack_get_back(s);
      stack_pop_back(s);
      qfront->operand1 = stack_get_back(s);
      stack_pop_back(s);
      stack_push_back(s, qfront);
    }
    else
    {
      stack_push_back(s, qfront);
    }
  }

  return stack_get_back(s);
}

AST_T* parser_parse_return_statement(parser_T* parser)
{
  parser_eat(parser, TOKEN_ID); // return
  AST_T* return_ast = init_ast(AST_RETURN_STATEMENT);
  return_ast->return_statement = parser_parse_expr(parser);

  return return_ast;
}

AST_T* parser_parse_id(parser_T* parser)
{
  if(strcmp(parser->current_token->value, "var") == 0)
  {
    return parser_parse_variable_definition(parser);
  }
  else if(strcmp(parser->current_token->value, "function") == 0)
  {
    return parser_parse_function_definition(parser);
  }
  else if(strcmp(parser->current_token->value, "if") == 0)
  {
    return parser_parse_if_statement(parser);
  }
  else if(strcmp(parser->current_token->value, "while") == 0)
  {
    return parser_parse_while_loop(parser);
  }
  else if(strcmp(parser->current_token->value, "for") == 0)
  {
    return parser_parse_for_loop(parser);
  }
  else if(strcmp(parser->current_token->value, "true") == 0)
  {
    parser_eat(parser, TOKEN_ID); // true
    AST_T* ast = init_ast(AST_BOOLEAN);
    ast->boolean_value = true;

    // if(parser->current_token->type == TOKEN_BINARY_OPERATOR)
    // {
    //   parsed_expr_stack_push_back(parser, ast);
    //   return parser_parse_expr(parser);
    // }

    return ast;
  }
  else if(strcmp(parser->current_token->value, "false") == 0)
  {
    parser_eat(parser, TOKEN_ID); // false
    AST_T* ast = init_ast(AST_BOOLEAN);
    ast->boolean_value = false;

    // if(parser->current_token->type == TOKEN_BINARY_OPERATOR)
    // {
    //   parsed_expr_stack_push_back(parser, ast);
    //   return parser_parse_expr(parser);
    // }

    return ast;
  }
  else if(strcmp(parser->current_token->value, "return") == 0)
  {
    return parser_parse_return_statement(parser);
  }
  else
  {
    return parser_parse_variable(parser);
  } 
}

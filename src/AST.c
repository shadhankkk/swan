#include "include/AST.h"
#include <string.h>
#include <stdio.h>

AST_T* init_ast(int type)
{
  AST_T* ast = calloc(1, sizeof(struct AST_STRUCT));
  ast->type = type;

  /* AST_VARIABLE_DEFINITION */
  ast->variable_definition_variable_name = (void*) 0;
  ast->variable_definition_value = (void*) 0;
  struct AST_STRUCT* original_variable_definition_value = (void*) 0;

  /* AST_FUNCTION_DEFINITION */
  ast->function_definition_body = (void*) 0;
  ast->function_definition_name = (void*) 0;
  ast->function_definition_arguments_size = 0;
  ast->function_definition_arguments = (void*) 0;

  /* AST_VARIABLE */
  ast->variable_name = (void*) 0;

  /* AST_ASSIGNMENT */
  ast->assignment_variable = (void*) 0;
  ast->new_assignment_value = (void*) 0;

  /* AST_ARRAY_ACCESS */
  ast->array_access_array = (void*) 0;
  ast->array_access_index = (void*) 0;

  /* AST_ARRAY_ASSIGNMENT */
  ast->array_assignment_array= (void*) 0;
  ast->array_assignment_index = (void*) 0;
  ast->new_array_assignment_value = (void*) 0;

  /* AST_IF_STATEMENT */
  ast->if_statement_predicate = (void*) 0;
  ast->if_statement_body = (void*) 0;

  /* AST_ELSE_IF STATEMENT */
  ast->else_statement = (void*) 0;

  /* AST_WHILE_LOOP */
  ast->while_loop_predicate = (void*) 0;
  ast->while_loop_body = (void*) 0;

  /* AST_FOR_LOOP */
  ast->for_loop_var_definition = (void*) 0;
  ast->for_loop_predicate = (void*) 0;
  ast->for_loop_assignment = (void*) 0;
  ast->for_loop_body = (void*) 0;

  /* AST_FUNCTION_CALL */
  ast->function_call_name = (void*) 0;
  ast->function_call_arguments = (void*) 0;
  ast->function_call_arguments_size = 0;

  /* AST_STRING */
  ast->string_value = (void*) 0;

  /* AST_LITERAL */
  ast->literal_value = 0;

  /* AST_BOOLEAN */
  ast->boolean_value = false;

  /* AST_ARRAY */
  ast->array = (void*) 0;
  ast->array_size = 0;

  /* AST_COMPOUND */
  ast->compound_value = (void*) 0;
  ast->compound_size = 0;

  /* AST_UNARY_OPERATOR */
  ast->unary_operator = (void*) 0;
  ast->unary_operand = (void*) 0;

  /* AST_BINARY_OPERATOR */
  ast->binary_operator = (void*) 0;
  ast->operand1 = (void*) 0;
  ast->operand2 = (void*) 0;


  /* AST_RETURN_STATEMENT */
  ast->return_statement = (void*) 0;

  return ast;
}

AST_T* ast_copy(AST_T* ast)
{
  AST_T* res;
  if(ast->type == AST_LITERAL)
  {
    res = init_ast(AST_LITERAL);
    res->literal_value = ast->literal_value;
    return res;
  }
  else if(ast->type == AST_BOOLEAN)
  {
    res = init_ast(AST_BOOLEAN);
    res->boolean_value = ast->boolean_value;
    return res;
  }
  else if(ast->type == AST_STRING)
  {
    res = init_ast(AST_STRING);
    res->string_value = calloc(
      strlen(ast->string_value) + 1,
      sizeof(char)
    );
    strcpy(res->string_value, ast->string_value);
    return res;
  }
  else if(ast->type == AST_ARRAY)
  {
    res = init_ast(AST_ARRAY);
    res->array_size = ast->array_size;
    res->array = calloc(
      res->array_size,
      sizeof(struct AST_STRUCT*)
    );

    for(int i=0; i<res->array_size; ++i)
    {
      res->array[i] = ast_copy(ast->array[i]);
    }

    return res;
  }
  else if(ast->type == AST_VARIABLE_DEFINITION)
  {
    res = init_ast(AST_VARIABLE_DEFINITION);
    res->variable_definition_value = ast_copy(ast->variable_definition_value);
    res->variable_definition_variable_name = calloc(strlen(ast->variable_definition_variable_name) + 1, sizeof(char));
    res->variable_definition_variable_name = strcpy(res->variable_definition_variable_name, ast->variable_definition_variable_name);

    return res;
  }
  else
  {
    return init_ast(AST_NOOP);
  }
}
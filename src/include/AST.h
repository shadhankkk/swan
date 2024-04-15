#ifndef AST_H
#define AST_H
#include <stdlib.h>
#include <stdbool.h>

typedef struct AST_STRUCT
{
  enum {
    AST_VARIABLE_DEFINITION,
    AST_FUNCTION_DEFINITION,
    AST_VARIABLE,
    AST_ASSIGNMENT,
    AST_ARRAY_ACCESS,
    AST_ARRAY_ASSIGNMENT,
    AST_IF_STATEMENT,
    AST_ELSE_IF_STATEMENT,
    AST_WHILE_LOOP,
    AST_FOR_LOOP,
    AST_FUNCTION_CALL,
    AST_STRING,
    AST_LITERAL,
    AST_BOOLEAN,
    AST_ARRAY,
    AST_COMPOUND,
    AST_UNARY_OPERATOR,
    AST_BINARY_OPERATOR,
    AST_LBRACE,
    AST_RBRACE,
    AST_RETURN_STATEMENT,
    AST_END_OF_FUNCTION,
    AST_NOOP,
  } type;

  /* AST_VARIABLE_DEFINITION */
  char* variable_definition_variable_name;
  struct AST_STRUCT* variable_definition_value;
 
  /* AST_FUNCTION_DEFINITION */
  struct AST_STRUCT* function_definition_body;
  char* function_definition_name;
  char** function_definition_arguments;
  int function_definition_arguments_size;

  /* AST_VARIABLE */
  char* variable_name;

  /* AST_ASSIGNMENT */
  struct AST_STRUCT* assignment_variable;
  struct AST_STRUCT* new_assignment_value;

  /* AST_ARRAY_ACCESS */
  char* array_access_variable_name;
  struct AST_STRUCT* array_access_index;

  /* AST_ARRAY_ASSIGNMENT */
  char* array_assignment_variable_name;
  struct AST_STRUCT* array_assignment_index;
  struct AST_STRUCT* new_array_assignment_value;

  /* AST_IF_STATEMENT */
  struct AST_STRUCT* if_statement_predicate;
  struct AST_STRUCT* if_statement_body;

  /* AST_ELSE_IF STATEMENT */
  struct AST_STRUCT* else_statement;

  /* AST_WHILE_LOOP */
  struct AST_STRUCT* while_loop_predicate;
  struct AST_STRUCT* while_loop_body;

  /* AST_FOR_LOOP */
  struct AST_STRUCT* for_loop_var_definition;
  struct AST_STRUCT* for_loop_predicate;
  struct AST_STRUCT* for_loop_assignment;
  struct AST_STRUCT* for_loop_body;

  /* AST_FUNCTION_CALL */
  char* function_call_name;
  struct AST_STRUCT** function_call_arguments;
  size_t function_call_arguments_size;

  /* AST STRING */
  char* string_value;

  /* AST_LITERAL */
  long long literal_value;

  /* AST BOOLEAN */
  bool boolean_value;

  /* AST_ARRAY */
  struct AST_STRUCT** array;
  size_t array_size;

  /* AST_COMPOUND */
  struct AST_STRUCT** compound_value;
  size_t compound_size;

  /* AST_UNARY_OPERATOR */
  char* unary_operator;
  struct AST_STRUCT* unary_operand;

  /* AST_BINARY_OPERATOR */
  char* binary_operator;
  struct AST_STRUCT* operand1;
  struct AST_STRUCT* operand2;

  /* AST_RETURN_STATEMENT */
  struct AST_STRUCT* return_statement;

  /* AST_END_OF_FUNCTION */

} AST_T;


AST_T* init_ast(int type);
#endif

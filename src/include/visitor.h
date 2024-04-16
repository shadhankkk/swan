#ifndef VISITOR_H
#define VISITOR_H
#include "AST.h"
#include "env_node.h"

typedef struct VISITOR_STRUCT
{
  env_node_T* env_list_head;
  bool is_returning;
  AST_T* return_value;
  // AST_T** variable_definitions;
  // size_t variable_definitions_size;
} visitor_T;

visitor_T* init_visitor();

visitor_T* create_visitor_copy_with_new_env(visitor_T* visitor, env_node_T* env_node);

AST_T* visitor_visit(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_variable_definition(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_function_definition(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_variable(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_assignment(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_array_access(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_array_assignment(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_if_statement(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_while_loop(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_for_loop(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_function_call(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_string(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_literal(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_boolean(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_array(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_compound(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_unary_operator(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_binary_operator(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_lbrace(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_rbrace(visitor_T* visitor, AST_T* node);

AST_T* visitor_visit_return_statement(visitor_T* visitor, AST_T* node);

/* LINEAR ALGEBRA STUFF */
AST_T* visitor_visit_matrix_addition(visitor_T* visitor, AST_T* m1, AST_T* m2);

AST_T* visitor_visit_matrix_subtraction(visitor_T* visitor, AST_T* m1, AST_T* m2);

AST_T* visitor_visit_matrix_multiplication(visitor_T* visitor, AST_T* m1, AST_T* m2);

#endif

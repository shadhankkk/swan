#ifndef ENV_NODE_H
#define ENV_NODE_H

#include "AST.h"

typedef struct ENV_NODE_STRUCT
{
  AST_T** variable_definitions;
  size_t variable_definitions_size;
  
  AST_T** function_definitions;
  size_t function_definitions_size;

  struct ENV_NODE_STRUCT* next;
} env_node_T;

env_node_T* init_env_node();

env_node_T* add_variable_definition_to_env_node(AST_T* variable_definition, env_node_T* env_node);

#endif
#include "include/env_node.h"
#include "include/visitor.h"
#include <stdio.h>
#include <string.h>

env_node_T* init_env_node()
{
  env_node_T* env_node = calloc(1, sizeof(struct ENV_NODE_STRUCT));

  env_node->variable_definitions = (void*) 0;
  env_node->variable_definitions_size = 0;

  env_node->function_definitions = (void*) 0;
  env_node->function_definitions_size = 0;

  env_node->next = (void*) 0;

  return env_node;
}

env_node_T* add_variable_definition_to_env_node(AST_T* variable_definition, env_node_T* env_node)
{
  if(env_node->variable_definitions == (void*) 0)
  {
    env_node->variable_definitions = calloc(1, sizeof(struct AST_STRUCT*));
    ++env_node->variable_definitions_size;
    env_node->variable_definitions[0] = ast_copy(variable_definition);
    //env_node->variable_definitions[0]->variable_definition_value = ast_copy(variable_definition->variable_definition_value);
  }
  else
  {
    
    for(int i=0; i<env_node->variable_definitions_size; ++i)
    {
      if(strcmp(env_node->variable_definitions[i]->variable_definition_variable_name, variable_definition->variable_definition_variable_name) == 0)
      {
        printf("Variable %s has already been defined in this scope\n", env_node->variable_definitions[i]->variable_definition_variable_name);
        exit(1);
      }
    }
    
    ++env_node->variable_definitions_size;
    env_node->variable_definitions = realloc(
      env_node->variable_definitions,
      env_node->variable_definitions_size * sizeof(struct AST_STRUCT*)
    );

    env_node->variable_definitions[env_node->variable_definitions_size - 1] = ast_copy(variable_definition);
    //env_node->variable_definitions[env_node->variable_definitions_size - 1]->variable_definition_value = variable_definition->original_variable_definition_value;
  }

  return env_node;
}
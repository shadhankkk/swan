#include "include/visitor.h"
#include <stdio.h>
#include <string.h>

visitor_T* init_visitor()
{
  visitor_T* visitor = calloc(1, sizeof(struct VISITOR_STRUCT));
  visitor->env_list_head = init_env_node();
  visitor->is_returning = false;
  visitor->return_value = (void*) 0;

  return visitor;
}

visitor_T* create_visitor_copy_with_new_env(visitor_T* visitor, env_node_T* env_node)
{
  visitor_T* visitor_copy = calloc(1, sizeof(struct VISITOR_STRUCT));
  visitor_copy->env_list_head = env_node;

  return visitor_copy;
}

/* builtin functions */

static AST_T* builtin_function_print(visitor_T* visitor, AST_T** args, int args_size, char* newline)
{
  for(int i = 0; i < args_size; ++i)
  {
    AST_T* visited_ast = visitor_visit(visitor, args[i]);

    switch (visited_ast->type)
    {
      case AST_STRING: printf("%s%s", visited_ast->string_value, newline); break;
      case AST_LITERAL: printf("%d%s", visited_ast->literal_value, newline); break;
      case AST_BOOLEAN: visited_ast->boolean_value ? printf("true%s", newline) : printf("false%s", newline); break;
      case AST_ARRAY: printf("\n[\n"); builtin_function_print(visitor, visited_ast->array, visited_ast->array_size, " "); printf("\n]\n"); break;
      default: printf("print fail at mem address: %p%s", visited_ast, newline); break;
    }

  }

  return init_ast(AST_NOOP);
}

static AST_T* builtin_function_length(visitor_T* visitor, AST_T** args, int args_size)
{
  if(args_size != 1)
  {
    printf("Expected %d arguments, got %d arguments", 1, args_size);
    exit(1);
  }

  AST_T* argument = visitor_visit(visitor, args[0]);
  AST_T* res = init_ast(AST_LITERAL);

  if(argument->type == AST_ARRAY)
  {
    res->literal_value = argument->array_size;
    return res;
  }
  else if(argument->type == AST_STRING)
  {
    res->literal_value = strlen(argument->string_value);
    return res;
  }
  else
  {
    printf("Wrong argument type for length function\n");
    exit(1);

    return init_ast(AST_NOOP);
  }
}

static AST_T* builtin_function_push(visitor_T* visitor, AST_T** args, int args_size)
{
  if(args_size != 2)
  {
    printf("Expected %d arguments, got %d arguments", 2, args_size);
    exit(1);
  }

  args[0] = visitor_visit(visitor, args[0]);
  args[1] = visitor_visit(visitor, args[1]);

  if(args[0]->type == AST_ARRAY)
  {
    ++args[0]->array_size;
    args[0]->array = realloc(
      args[0]->array,
      args[0]->array_size * sizeof(struct AST_STRUCT*)
    );

    args[0]->array[args[0]->array_size - 1] = args[1];

    return args[0];
  }
  else
  {
     printf("Wrong argument type for push function\n");
    exit(1);

    return init_ast(AST_NOOP);
  }
}

static AST_T* builtin_function_append(visitor_T* visitor, AST_T** args, int args_size)
{
  // append(array, value);
  if(args_size != 2)
  {
    printf("Expected %d arguments, got %d arguments", 2, args_size);
    exit(1);
  }

  args[0] = visitor_visit(visitor, args[0]);
  args[1] = visitor_visit(visitor, args[1]);

  if(args[0]->type == AST_ARRAY)
  {

    if(args[1]->type != AST_ARRAY)
    {
      printf("Expected array type for 2nd argument in append\n");
      exit(1);
    }

    AST_T* res_ast = init_ast(AST_ARRAY);

    res_ast->array = calloc(
      (args[0]->array_size + args[1]->array_size),
      sizeof(struct AST_STRUCT*)
    );
    res_ast->array_size = (args[0]->array_size + args[1]->array_size);

    for(int i=0; i<args[0]->array_size; ++i)
    {
      res_ast->array[i] = ast_copy(visitor_visit(visitor, args[0]->array[i]));
    }
    
    for(int i=args[0]->array_size; i < (args[0]->array_size + args[1]->array_size); ++i)
    {
      res_ast->array[i] = ast_copy(visitor_visit(visitor, args[1]->array[i - args[0]->array_size]));
    }

    return res_ast;
  }
  else if(args[0]->type == AST_STRING)
  {
    if(args[1]->type != AST_STRING)
    {
      printf("Expected string type for 2nd argument in append\n");
      exit(1);
    }
    args[0]->string_value = strcat(args[0]->string_value, args[1]->string_value);

    return args[0];
  }
  else{
     printf("Wrong argument type for append function\n");
    exit(1);

    return init_ast(AST_NOOP);
  }
}

static AST_T* builtin_function_Matrix(visitor_T* visitor, AST_T** args, int args_size)
{
  if(args_size == 2)
  {
    AST_T* rows = visitor_visit(visitor, args[0]);
    AST_T* cols = visitor_visit(visitor, args[1]);

    if(rows->type != AST_LITERAL || cols->type != AST_LITERAL)
    {
      printf("Matrix() arguments must be an integer!\n");
      exit(1);
    }

    AST_T* mat_ast = init_ast(AST_ARRAY);
    mat_ast->array_size = rows->literal_value;

    mat_ast->array = calloc(
      rows->literal_value,
      sizeof(struct AST_STRUCT*)
    );

    for(int i=0; i<rows->literal_value; ++i)
    {
      mat_ast->array[i] = init_ast(AST_ARRAY);
      mat_ast->array[i]->array_size = cols->literal_value;
      mat_ast->array[i]->array = calloc(
        cols->literal_value,
        sizeof(struct AST_STRUCT*)
      );
      for(int j=0; j< cols->literal_value; ++j)
      {
        mat_ast->array[i]->array[j] = init_ast(AST_LITERAL);
        mat_ast->array[i]->array[j]->literal_value = 0;
      }
    }

    return mat_ast;
  }
  return init_ast(AST_NOOP);
}

AST_T* isAllColsSameSize(AST_T* ast_array)
{

  if(ast_array->array[0]->type != AST_ARRAY)
  {
    printf("Argument is not a matrix!");
    exit(1);
  }

  size_t firstColSize = ast_array->array[0]->array_size;
  
  for(int i=0; i<ast_array->array_size; ++i)
  {
    if(ast_array->array[i]->type != AST_ARRAY)
    {
      printf("Argument is not a matrix!");
      exit(1);
    }

    if(ast_array->array[i]->array_size != firstColSize)
    {
      return false;
    }
  }

  return true;
}

AST_T* visitor_visit(visitor_T* visitor, AST_T* node)
{
  switch (node->type)
  {
    case AST_VARIABLE_DEFINITION: return visitor_visit_variable_definition(visitor, node); break;
    case AST_FUNCTION_DEFINITION: return visitor_visit_function_definition(visitor, node); break;
    case AST_VARIABLE: return visitor_visit_variable(visitor, node); break;
    case AST_ASSIGNMENT: return visitor_visit_assignment(visitor, node); break;
    case AST_ARRAY_ACCESS: return visitor_visit_array_access(visitor, node); break;
    case AST_ARRAY_ASSIGNMENT: return visitor_visit_array_assignment(visitor, node); break;
    case AST_FUNCTION_CALL: return visitor_visit_function_call(visitor, node); break;
    case AST_STRING: return visitor_visit_string(visitor, node); break;
    case AST_LITERAL: return visitor_visit_literal(visitor, node); break;
    case AST_BOOLEAN: return visitor_visit_boolean(visitor, node); break;
    case AST_COMPOUND: return visitor_visit_compound(visitor, node); break; 
    case AST_UNARY_OPERATOR: return visitor_visit_unary_operator(visitor, node); break; 
    case AST_BINARY_OPERATOR: return visitor_visit_binary_operator(visitor, node); break;
    case AST_LBRACE: return visitor_visit_lbrace(visitor, node); break;
    case AST_RBRACE: return visitor_visit_rbrace(visitor, node); break;
    case AST_ARRAY: return visitor_visit_array(visitor, node); break;
    case AST_IF_STATEMENT: return visitor_visit_if_statement(visitor, node); break;
    case AST_WHILE_LOOP: return visitor_visit_while_loop(visitor, node); break;
    case AST_FOR_LOOP: return visitor_visit_for_loop(visitor, node); break;
    case AST_RETURN_STATEMENT: return visitor_visit_return_statement(visitor, node); break;
    case AST_END_OF_FUNCTION: return node; break;
    case AST_NOOP: return node; break;
  }

  printf("Uncaught statement of type %d\n", node->type);
  exit(1);

  return init_ast(AST_NOOP);
}

AST_T* visitor_visit_lbrace(visitor_T* visitor, AST_T* node)
{
  env_node_T* new_env = init_env_node();
  new_env->next = visitor->env_list_head;
  visitor->env_list_head = new_env;

  return node;
}

AST_T* visitor_visit_rbrace(visitor_T* visitor, AST_T* node)
{
  visitor->env_list_head = visitor->env_list_head->next;
  return node;
}

AST_T* visitor_visit_variable_definition(visitor_T* visitor, AST_T* node)
{
  node->variable_definition_value = visitor_visit(visitor, node->variable_definition_value);
  visitor->env_list_head = add_variable_definition_to_env_node(node, visitor->env_list_head);
  
  return node;
}

AST_T* visitor_visit_function_definition(visitor_T* visitor, AST_T* node)
{
  
  env_node_T* curr_env = visitor->env_list_head;

  ++curr_env->function_definitions_size;

  if(curr_env->function_definitions == (void*) 0)
  {
    curr_env->function_definitions = calloc(1, sizeof(struct AST_T*));
    curr_env->function_definitions[0] = node;
  }
  else
  {
    curr_env->function_definitions = realloc(
      curr_env->function_definitions,
      curr_env->function_definitions_size * sizeof(struct AST_T*)
    );
    
    curr_env->function_definitions[curr_env->function_definitions_size - 1] = node;
  }

  return node;
}

AST_T* visitor_visit_variable(visitor_T* visitor, AST_T* node)
{
  env_node_T* curr_env = visitor->env_list_head;
  while(curr_env != (void*) 0)
  {
    for(int i=0; i<curr_env->variable_definitions_size; ++i)
    {
      AST_T* vardef = curr_env->variable_definitions[i];
      
      if(strcmp(vardef->variable_definition_variable_name, node->variable_name) == 0)
      {
        return visitor_visit(visitor, vardef->variable_definition_value);
      }
      
    }

    curr_env = curr_env->next;
  }

  printf("Undefined variable '%s'\n", node->variable_name);
  exit(1);

  return node;
}

AST_T* visitor_visit_assignment(visitor_T* visitor, AST_T* node)
{
  env_node_T* curr_env = visitor->env_list_head;
  while(curr_env!= (void*) 0)
  {
    for(int i=0; i<curr_env->variable_definitions_size; ++i)
    {
      AST_T* vardef = curr_env->variable_definitions[i];
      if(strcmp(vardef->variable_definition_variable_name, node->assignment_variable->variable_name) == 0)
      {
        vardef->variable_definition_value = ast_copy(visitor_visit(visitor, node->new_assignment_value));
        return vardef->variable_definition_value;
      }
    }

    curr_env = curr_env->next;
  }

  printf("Undefined variable '%s'\n", node->variable_name);
  exit(1);

  return node;
}

AST_T* visitor_visit_array_access(visitor_T* visitor, AST_T* node)
{
  AST_T* index = visitor_visit(visitor, node->array_access_index);
  if(index->type != AST_LITERAL)
  {
    printf("Array index must be a literal\n");
    exit(1);
  }
  // AST_T* variable_ast = init_ast(AST_VARIABLE);
  // variable_ast->variable_name = node->array_access_variable_name;
  // AST_T* array_ast = visitor_visit(visitor, variable_ast);
  AST_T* array_ast = node->array_access_array;
  array_ast = visitor_visit(visitor, array_ast);
  if(array_ast->type != AST_ARRAY)
  {
    printf("argument is not an array\n");
    exit(1);
  }

  if(index->literal_value >= array_ast->array_size)
  {
    printf("index %d is out of bounds for array\n", index->literal_value);
    exit(1);
  }

  return array_ast->array[index->literal_value];
}

AST_T* visitor_visit_array_assignment(visitor_T* visitor, AST_T* node)
{
  AST_T* index = visitor_visit(visitor, node->array_assignment_index);
  if(index->type != AST_LITERAL)
  {
    printf("Array index must be a literal\n");
    exit(1);
  }
  
  // AST_T* variable_ast = init_ast(AST_VARIABLE);
  // variable_ast->variable_name = node->array_assignment_variable_name;
  // AST_T* array_ast = visitor_visit(visitor, variable_ast);
  AST_T* array_ast = node->array_assignment_array;
  array_ast = visitor_visit(visitor, array_ast);
  
  if(array_ast->type != AST_ARRAY)
  {
    printf("argument is not an array\n");
    exit(1);
  }

  if(index->literal_value >= array_ast->array_size)
  {
    printf("index %d is out of bounds for array\n", index->literal_value);
    exit(1);
  }
  
  AST_T* new_assignment_value = ast_copy(visitor_visit(visitor, node->new_array_assignment_value));
  array_ast->array[index->literal_value] = new_assignment_value;

  return node;
}

AST_T* visitor_visit_if_statement(visitor_T* visitor, AST_T* node)
{
  AST_T* predicate= visitor_visit(visitor, node->if_statement_predicate);
  if(predicate->boolean_value == true || predicate->literal_value != 0)
  {
    visitor_visit_lbrace(visitor, node);
    visitor_visit(visitor, node->if_statement_body);
    visitor_visit_rbrace(visitor, node);
  } else if(node->else_statement != (void*) 0) {
    visitor_visit(visitor, node->else_statement);
  }

  return node;
}

AST_T* visitor_visit_while_loop(visitor_T* visitor, AST_T* node)
{ 
  visitor_visit_lbrace(visitor, node);
  AST_T* predicate = visitor_visit(visitor, node->while_loop_predicate);
  while(predicate->type == AST_BOOLEAN && predicate->boolean_value == true)
  {
    visitor_visit_lbrace(visitor, node);
    visitor_visit(visitor, node->while_loop_body);
    visitor_visit_rbrace(visitor, node);
    predicate = visitor_visit(visitor, node->while_loop_predicate);
  }

  visitor_visit_rbrace(visitor, node);

  return node;
}

AST_T* visitor_visit_for_loop(visitor_T* visitor, AST_T* node)
{
  visitor_visit_lbrace(visitor, node);
  AST_T* var_definition = visitor_visit(visitor, node->for_loop_var_definition);
  AST_T* predicate = visitor_visit(visitor, node->for_loop_predicate);
  AST_T* assignment = node->for_loop_assignment;
  while(predicate->boolean_value == true)
  {
    visitor_visit_lbrace(visitor, node);
    visitor_visit(visitor, node->for_loop_body);
    visitor_visit_rbrace(visitor, node);
    visitor_visit(visitor, assignment);
    predicate = visitor_visit(visitor, node->for_loop_predicate);
  }

  visitor_visit_rbrace(visitor, node);

  return node;
}

AST_T* visitor_visit_function_call(visitor_T* visitor, AST_T* node)
{
  if(strcmp(node->function_call_name, "print") == 0)
  {
    return builtin_function_print(visitor, node->function_call_arguments, node->function_call_arguments_size, "\n");
  }

  if(strcmp(node->function_call_name, "length") == 0)
  {
    return builtin_function_length(visitor, node->function_call_arguments, node->function_call_arguments_size);
  }

  if(strcmp(node->function_call_name, "append") == 0)
  {
    return builtin_function_append(visitor, node->function_call_arguments, node->function_call_arguments_size);
  }

  if(strcmp(node->function_call_name, "push") == 0)
  {
    return builtin_function_push(visitor, node->function_call_arguments, node->function_call_arguments_size);
  }

  if(strcmp(node->function_call_name, "Matrix") == 0)
  {
    return builtin_function_Matrix(visitor, node->function_call_arguments, node->function_call_arguments_size);
  }

  env_node_T* curr_env = visitor->env_list_head;
  
  while(curr_env != (void*) 0)
  {
    for(int i=0; i<curr_env->function_definitions_size; ++i)
    {
      AST_T* curr_function_definition = curr_env->function_definitions[i];
      
      if(strcmp(curr_function_definition->function_definition_name, node->function_call_name) == 0)
      {
        if(node->function_call_arguments_size != curr_function_definition->function_definition_arguments_size)
        {
          printf("Expected %d arguments, got %d arguments\n", 
                 curr_function_definition->function_definition_arguments_size, 
                 node->function_call_arguments_size);
          exit(1);
        }

        env_node_T* function_env = init_env_node();
        function_env->next = curr_env;

        visitor_T* function_visitor = create_visitor_copy_with_new_env(visitor, function_env);

        for(int j=0; j<curr_function_definition->function_definition_arguments_size; ++j)
        {
          char* argument_symbol = curr_function_definition->function_definition_arguments[j];
          AST_T* argument_value = visitor_visit(function_visitor, node->function_call_arguments[j]);
          
          AST_T* argument_bound_variable = init_ast(AST_VARIABLE_DEFINITION);
          argument_bound_variable->variable_definition_variable_name = argument_symbol;
          argument_bound_variable->variable_definition_value = argument_value;
        
          visitor_visit_variable_definition(function_visitor, argument_bound_variable);
        }

        return visitor_visit(function_visitor, curr_function_definition->function_definition_body);
      }
    }

    curr_env = curr_env->next;
  }

  printf("Undefined method '%s'\n", node->function_call_name);
  exit(1);

}

AST_T* visitor_visit_string(visitor_T* visitor, AST_T* node)
{
  return node;
}

AST_T* visitor_visit_literal(visitor_T* visitor, AST_T* node)
{
  return node;
}

AST_T* visitor_visit_boolean(visitor_T* visitor, AST_T* node)
{
  return node;
}

AST_T* visitor_visit_array(visitor_T* visitor, AST_T* node)
{
  for(int i=0; i<node->array_size; ++i)
  {
    node->array[i] = visitor_visit(visitor, node->array[i]);
  }

  return node;
}

AST_T* visitor_visit_compound(visitor_T* visitor, AST_T* node)
{
  for(int i=0; i<node->compound_size; ++i)
  {
    if(visitor->is_returning)
    {
      if(node->compound_value[i]->type == AST_END_OF_FUNCTION)
      {
        visitor->is_returning = false;
        return visitor->return_value;
      }

      continue;
    }

  
    visitor_visit(visitor, node->compound_value[i]);
  }

  return init_ast(AST_NOOP);
}

AST_T* visitor_visit_unary_operator(visitor_T* visitor, AST_T* node)
{
  AST_T* unary_operand = visitor_visit(visitor, node->unary_operand);
  char* operator = node->unary_operator;
  if(strcmp(operator, "!") == 0)
  {
    AST_T* res = init_ast(AST_BOOLEAN);
    res->boolean_value = !unary_operand->boolean_value;

    return res;
  }

  return init_ast(AST_NOOP);
}

AST_T* visitor_visit_binary_operator(visitor_T* visitor, AST_T* node)
{
  AST_T* operand1 = visitor_visit(visitor, node->operand1);
  AST_T* operand2 = visitor_visit(visitor, node->operand2);

  char* operator = node->binary_operator;

  if(strcmp(operator, ">") == 0)
  {
    AST_T* res = init_ast(AST_BOOLEAN);
    if(operand1->literal_value > operand2->literal_value)
    {
      res->boolean_value = true;
    }

    return res;
  }
  else if(strcmp(operator, "<") == 0)
  {
    AST_T* res = init_ast(AST_BOOLEAN);
    if(operand1->literal_value < operand2->literal_value)
    {
      res->boolean_value = true;
    }

    return res;
  }
  else if(strcmp(operator, "==") == 0)
  {
    AST_T* res = init_ast(AST_BOOLEAN);
    if(operand1->type == AST_LITERAL && operand2->type == AST_LITERAL)
    {
      res->boolean_value = (operand1->literal_value == operand2->literal_value);
    }
    else if(operand1->type == AST_BOOLEAN && operand2->type == AST_BOOLEAN)
    {
      res->boolean_value = (operand1->boolean_value == operand2->boolean_value);
    }
    else if(operand1->type == AST_STRING && operand2->type == AST_STRING)
    {
      res->boolean_value = (strcmp(operand1->string_value, operand2->string_value) == 0);
    }
    else
    {
      printf("Incompatible types for == operator\n");
      exit(1);
    }

    return res;
  }
  else if(strcmp(operator, "+") == 0)
  {
    if(operand1->type == AST_LITERAL && operand2->type == AST_LITERAL)
    {
      AST_T* res = init_ast(AST_LITERAL);
      res->literal_value = operand1->literal_value + operand2->literal_value;
      return res;
    }

    if(operand1->type == AST_STRING && operand2->type == AST_LITERAL)
    {
      AST_T* res = init_ast(AST_STRING);
      res->string_value = calloc(1, sizeof(char));
      res->string_value[0] = '\0';
      res->string_value = strcat(res->string_value, operand1->string_value);
      int op2_len = snprintf(NULL, 0, "%d", operand2->literal_value);
      char* op2_str = calloc(op2_len + 1, sizeof(char));
      snprintf(op2_str, op2_len + 1, "%d", operand2->literal_value);
      res->string_value = strcat(res->string_value, op2_str);
      return res;
    }

    if(operand1->type == AST_LITERAL && operand2->type == AST_STRING)
    {
      AST_T* res = init_ast(AST_STRING);
      res->string_value = operand2->string_value;
      int op1_len = snprintf(NULL, 0, "%d", operand1->literal_value);
      char* op1_str = calloc(op1_len + 1, sizeof(char));
      snprintf(op1_str, op1_len + 1, "%d", operand1->literal_value);
      res->string_value = strcat(op1_str, res->string_value);
      return res;
    }

    if(operand1->type == AST_STRING && operand2->type == AST_STRING)
    {
      
      AST_T* res = init_ast(AST_STRING);
      res->string_value = calloc(1, sizeof(char));
      res->string_value[0] = '\0';
      res->string_value = strcat(res->string_value, operand1->string_value);
      res->string_value = strcat(res->string_value, operand2->string_value);
      return res;
    }

    if(operand1->type == AST_ARRAY && operand2->type == AST_ARRAY)
    {
      return visitor_visit_matrix_addition(visitor, operand1, operand2);
    }

    printf("Type mismatch for + operator\n");
    exit(1);

  }
  else if(strcmp(operator, "-") == 0)
  {
    if(operand1->type == AST_LITERAL && operand2->type == AST_LITERAL)
    {
      AST_T* res = init_ast(AST_LITERAL);
      res->literal_value = operand1->literal_value - operand2->literal_value;
      return res;
    }

    if(operand1->type == AST_ARRAY && operand2->type == AST_ARRAY)
    {
      return visitor_visit_matrix_subtraction(visitor, operand1, operand2);
    }

    printf("Type mismatch for - operator\n");
    exit(1);
  }
  else if(strcmp(operator, "*") == 0)
  {
    if(operand1->type == AST_LITERAL && operand2->type == AST_LITERAL)
    {
      AST_T* res = init_ast(AST_LITERAL);
      res->literal_value = operand1->literal_value * operand2->literal_value;

      return res;
    }

    if(operand1->type == AST_ARRAY && operand2->type == AST_ARRAY)
    {
      return visitor_visit_matrix_multiplication(visitor, operand1, operand2);
    }

    printf("Type mismatch for * operator\n");
    exit(1);
  }
  else if(strcmp(operator, "/") == 0)
  {
    AST_T* res = init_ast(AST_LITERAL);
    res->literal_value = operand1->literal_value / operand2->literal_value;

    return res;
  }
  else if(strcmp(operator, "&&") == 0)
  {
    AST_T* res = init_ast(AST_BOOLEAN);
    res->boolean_value = operand1->boolean_value && operand2->boolean_value;

    return res;
  }
  else if(strcmp(operator, "||") == 0)
  {
    AST_T* res = init_ast(AST_BOOLEAN);
    res->boolean_value = operand1->boolean_value || operand2->boolean_value;

    return res;
  }
  
  return init_ast(AST_NOOP);
}

AST_T* visitor_visit_return_statement(visitor_T* visitor, AST_T* node)
{
  visitor->is_returning = true;
  visitor->return_value = visitor_visit(visitor, node->return_statement);
  return visitor->return_value;
}

/* LINEAR ALGEBRA STUFF */
AST_T* visitor_visit_matrix_addition(visitor_T* visitor, AST_T* m1, AST_T* m2)
{
  AST_T* operand1 = m1;
  AST_T* operand2 = m2;

  if(operand1->array_size != operand2->array_size)
  {
    printf("Cannot add two arrays with different sizes");
    exit(1);
  }

  if(isAllColsSameSize(operand1) && isAllColsSameSize(operand2) && (operand1->array[0]->array_size == operand2->array[0]->array_size))
  {
    AST_T* res = init_ast(AST_ARRAY);
    size_t rows = operand1->array_size;
    size_t cols = operand1->array[0]->array_size;

    res->array_size = rows;
    res->array = calloc(
      rows,
      sizeof(struct AST_STRUCT*)
    );
    
    for(int i=0; i<rows; ++i)
    {
      res->array[i] = init_ast(AST_ARRAY);
      res->array[i]->array_size = cols;
      res->array[i]->array = calloc(
        cols,
        sizeof(struct AST_STRUCT*)
      );
      for(int j=0; j<cols; ++j)
      {
        res->array[i]->array[j] = init_ast(AST_LITERAL);
        res->array[i]->array[j]->literal_value = 0;
      }
    }

    for(int i=0; i<rows; ++i)
    {
      for(int j=0; j<cols; ++j) {
        AST_T* binop1 = visitor_visit(visitor, operand1->array[i]->array[j]);
        AST_T* binop2 = visitor_visit(visitor, operand2->array[i]->array[j]);
        AST_T* binop = init_ast(AST_BINARY_OPERATOR);
        binop->binary_operator = "+";
        binop->operand1 = binop1;
        binop->operand2 = binop2;
        res->array[i]->array[j] = visitor_visit(visitor, binop);
      }
    }

    return res;
  }
  else
  {
    printf("Matrices must be the same size in order to add");
    exit(1);
  }
}

AST_T* visitor_visit_matrix_subtraction(visitor_T* visitor, AST_T* m1, AST_T* m2)
{
  AST_T* operand1 = m1;
  AST_T* operand2 = m2;

  if(operand1->array_size != operand2->array_size)
  {
    printf("Cannot subtract two arrays with different sizes");
    exit(1);
  }

  if(isAllColsSameSize(operand1) && isAllColsSameSize(operand2) && (operand1->array[0]->array_size == operand2->array[0]->array_size))
  {
    AST_T* res = init_ast(AST_ARRAY);
    size_t rows = operand1->array_size;
    size_t cols = operand1->array[0]->array_size;

    res->array_size = rows;
    res->array = calloc(
      rows,
      sizeof(struct AST_STRUCT*)
    );
    
    for(int i=0; i<rows; ++i)
    {
      res->array[i] = init_ast(AST_ARRAY);
      res->array[i]->array_size = cols;
      res->array[i]->array = calloc(
        cols,
        sizeof(struct AST_STRUCT*)
      );
      for(int j=0; j<cols; ++j)
      {
        res->array[i]->array[j] = init_ast(AST_LITERAL);
        res->array[i]->array[j]->literal_value = 0;
      }
    }

    for(int i=0; i<rows; ++i)
    {
      for(int j=0; j<cols; ++j) {
        AST_T* binop1 = visitor_visit(visitor, operand1->array[i]->array[j]);
        AST_T* binop2 = visitor_visit(visitor, operand2->array[i]->array[j]);
        AST_T* binop = init_ast(AST_BINARY_OPERATOR);
        binop->binary_operator = "-";
        binop->operand1 = binop1;
        binop->operand2 = binop2;
        res->array[i]->array[j] = visitor_visit(visitor, binop);
      }
    }

    return res;
  }
  else
  {
    printf("Matrices must be the same size in order to subtract");
    exit(1);
  }
}

AST_T* visitor_visit_matrix_multiplication(visitor_T* visitor, AST_T* m1, AST_T* m2)
{
  AST_T* operand1 = m1;
  AST_T* operand2 = m2;

  if(!isAllColsSameSize(operand1) || !isAllColsSameSize(operand2))
  {
    printf("All columns in matrices must be the same length\n");
    exit(1);
  }

  size_t op1_rows = m1->array_size;
  size_t op1_cols = m1->array[0]->array_size;

  size_t op2_rows = m2->array_size;
  size_t op2_cols = m2->array[0]->array_size;

  if(op1_cols != op2_rows)
  {
    printf("Cannot multiply two matrices when the first matrix doesn't have the same number of columns as the number of rows of the second matrix");
    exit(1);
  }

  AST_T* res = init_ast(AST_ARRAY);
  res->array_size = op1_rows;
  res->array = calloc(
    op1_rows,
    sizeof(struct AST_STRUCT*)
  );

  for(int i=0; i<op1_rows; ++i)
  {
    res->array[i] = init_ast(AST_ARRAY);
    res->array[i]->array_size = op2_cols;
    res->array[i]->array = calloc(
      op2_cols,
      sizeof(struct AST_STRUCT*)
    );

    for(int j=0; j<op2_cols; ++j)
    {
      res->array[i]->array[j] = (void*) 0;
    }

    for(int j=0; j<op2_cols; ++j)
    {
      for(int k=0; k<op2_rows; ++k)
      {
        AST_T* op1_elem = ast_copy(visitor_visit(visitor, operand1->array[i]->array[k]));
        AST_T* op2_elem = ast_copy(visitor_visit(visitor, operand2->array[k]->array[j]));

        AST_T* binop = init_ast(AST_BINARY_OPERATOR);
        binop->binary_operator = "*";
        binop->operand1 = op1_elem;
        binop->operand2 = op2_elem;
      
        AST_T* binop_res = ast_copy(visitor_visit(visitor, binop));
        
        if(res->array[i]->array[j] == (void*) 0)
        {
          res->array[i]->array[j] = binop_res;
        }
        else
        {
          AST_T* sumop = init_ast(AST_BINARY_OPERATOR);
          sumop->binary_operator = "+";
          sumop->operand1 = res->array[i]->array[j];
          sumop->operand2 = binop_res;
          res->array[i]->array[j] = ast_copy(visitor_visit(visitor, sumop));
        }
      }
    }
  }

  return res;
}
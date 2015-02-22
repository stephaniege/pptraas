#pragma once

struct IntNode
{
  int intData;
  struct IntNode *next;
  struct IntNode *prev;
};
typedef struct IntNode IntNode;

struct AccelNode
{
  AccelData accelData;
  struct AccelNode *next;
};
typedef struct AccelNode AccelNode;

IntNode* init_int_linked_list(uint);
void destroy_int_linked_list(IntNode*);

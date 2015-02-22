#include <pebble.h>
#include "accelerate.h"

IntNode* init_int_linked_list(uint num)
{
  if (num == 0) { return 0; }
  
  uint i = 0, successful = 0;

  IntNode* firstNode = malloc(sizeof(IntNode));
  if (firstNode != 0)
  {
    firstNode->intData = 0;
    firstNode->next = 0;
    successful++;
  }
  i++;
  
  IntNode* prevNode = firstNode;
  while (i < num)
  {
    IntNode* newNode = malloc(sizeof(IntNode));
    if (newNode != 0 && prevNode != 0)
    {
      newNode->intData = 0;
      newNode->next = 0;
      prevNode->next = newNode;
      prevNode = newNode;
      successful++;
    }
    i++;
  }
  
  char *infoFormatMsg = "%d integers allocated.";
  char infoMsg[32];
  snprintf(infoMsg, 32, infoFormatMsg, successful);
  APP_LOG(APP_LOG_LEVEL_INFO, infoMsg);
  return firstNode;
}

void destroy_int_linked_list(IntNode *toDestroy)
{
  uint successful = 0;
  while (toDestroy != 0)
  {
    IntNode *destroyed = toDestroy;
    toDestroy = toDestroy->next;
    free(destroyed);
    successful++;
  }
  
  char *infoFormatMsg = "%d integers deallocated.";
  char infoMsg[32];
  snprintf(infoMsg, 32, infoFormatMsg, successful);
  APP_LOG(APP_LOG_LEVEL_INFO, infoMsg);
}

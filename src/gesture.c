#include <pebble.h>
#include "gesture.h"

IntNode* init_int_linked_list(uint num)
{
  if (num == 0) { return 0; }
  
  uint i = 0, successful = 0;

  IntNode* firstNode = malloc(sizeof(IntNode));
  if (firstNode != 0)
  {
    firstNode->data = 0;
    firstNode->next = 0;
    firstNode->prev = 0;
    successful++;
  }
  i++;
  
  IntNode* prevNode = firstNode;
  while (i < num)
  {
    IntNode* newNode = malloc(sizeof(IntNode));
    if (newNode != 0)
    {
      newNode->data = 0;
      newNode->next = 0;
      newNode->prev = prevNode;
      prevNode->next = newNode;
      prevNode = newNode;
      successful++;
    }
    i++;
  }
  
  /* char *infoFormatMsg = "%d integers allocated.";
  char infoMsg[32];
  snprintf(infoMsg, 32, infoFormatMsg, successful);
  APP_LOG(APP_LOG_LEVEL_INFO, infoMsg); */
  return firstNode;
}

void destroy_int_linked_list(IntNode *to_destroy)
{
  uint successful = 0;
  while (to_destroy != 0)
  {
    IntNode *destroyed = to_destroy;
    to_destroy = to_destroy->next;
    free(destroyed);
    successful++;
  }
  
  /* char *infoFormatMsg = "%d integers deallocated.";
  char infoMsg[32];
  snprintf(infoMsg, 32, infoFormatMsg, successful);
  APP_LOG(APP_LOG_LEVEL_INFO, infoMsg); */
}

IntNode* get_last_node(IntNode* node)
{
  while(node->next != 0) { node = node->next; }
  return node;
}

// Adds a new integer to the linked list.
// Compensates by removing the first integer from the linked list.
IntNode* add_to_int_linked_list(IntNode* node, uint16_t data)
{
  // Precondition: The linked list has at least one node.
  IntNode* last_node = get_last_node(node);
  IntNode* new_node = malloc(sizeof(IntNode));
  new_node->data = data;
  new_node->next = 0;
  new_node->prev = last_node;
  last_node->next = new_node;
  
  IntNode* to_destroy = node;
  node = node->next;
  free(to_destroy);
  return node;
}

void init_data_streams(uint num)
{
  accel_data_x = init_int_linked_list(num);
  accel_data_y = init_int_linked_list(num);
  accel_data_z = init_int_linked_list(num);
  
  moving_avg_data_x = init_int_linked_list(num);
  moving_avg_data_y = init_int_linked_list(num);
  moving_avg_data_z = init_int_linked_list(num);
  
  difference_data_x = init_int_linked_list(num);
  difference_data_y = init_int_linked_list(num);
  difference_data_z = init_int_linked_list(num);
  
  last_large_change_x = init_int_linked_list(num);
  last_large_change_y = init_int_linked_list(num);
  last_large_change_z = init_int_linked_list(num);
}

void destroy_data_streams()
{
  destroy_int_linked_list(accel_data_x);
  destroy_int_linked_list(accel_data_y);
  destroy_int_linked_list(accel_data_z);
  
  destroy_int_linked_list(moving_avg_data_x);
  destroy_int_linked_list(moving_avg_data_y);
  destroy_int_linked_list(moving_avg_data_z);
  
  destroy_int_linked_list(difference_data_x);
  destroy_int_linked_list(difference_data_y);
  destroy_int_linked_list(difference_data_z);
  
  destroy_int_linked_list(last_large_change_x);
  destroy_int_linked_list(last_large_change_y);
  destroy_int_linked_list(last_large_change_z);
}

void add_accel_data_to_streams(AccelData *data)
{
  for (uint i = 0; i < NUM_SAMPLES; i++)
  {
    accel_data_x = add_to_int_linked_list(accel_data_x, data[i].x);
    accel_data_y = add_to_int_linked_list(accel_data_y, data[i].y);
    accel_data_z = add_to_int_linked_list(accel_data_z, data[i].z);
  }
}
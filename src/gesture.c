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
  
  /* static char infoMsg[32];
  snprintf(infoMsg, sizeof(infoMsg), "%d integers allocated.", successful);
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
  
  /* static char infoMsg[32];
  snprintf(infoMsg, sizeof(infoMsg), "%d integers deallocated.", successful);
  APP_LOG(APP_LOG_LEVEL_INFO, infoMsg); */
}

IntNode* get_last_node(IntNode* node)
{
  while(node->next != 0) { node = node->next; }
  return node;
}

// Adds a new integer to the linked list.
// Compensates by removing the first integer from the linked list.
IntNode* add_to_int_linked_list(IntNode* node, int16_t data)
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

void add_accel_data_to_streams(AccelData *data, TextLayer *s_debug_layer)
{
  int16_t last_accel_x = get_last_node(accel_data_x)->data;
  int16_t last_accel_y = get_last_node(accel_data_y)->data;
  int16_t last_accel_z = get_last_node(accel_data_z)->data;
  
  for (uint i = 0; i < NUM_SAMPLES; i++)
  {
    int16_t new_accel_x = data[i].x;
    int16_t new_accel_y = data[i].y;
    int16_t new_accel_z = data[i].z;
    
    // Add the acceleration data to the streams that store the original values.
    accel_data_x = add_to_int_linked_list(accel_data_x, new_accel_x);
    accel_data_y = add_to_int_linked_list(accel_data_y, new_accel_y);
    accel_data_z = add_to_int_linked_list(accel_data_z, new_accel_z);
    
    // Find the moving average along each direction, and add that
    // information to the moving average streams.
    moving_avg_data_x = add_new_moving_avg(moving_avg_data_x, last_accel_x, new_accel_x);
    moving_avg_data_y = add_new_moving_avg(moving_avg_data_y, last_accel_y, new_accel_y);
    moving_avg_data_z = add_new_moving_avg(moving_avg_data_z, last_accel_z, new_accel_z);
    
    // Find the difference in the moving average along each direction,
    // and add that information to the difference streams.
    IntNode* last_moving_avg_data_x = get_last_node(moving_avg_data_x);
    IntNode* last_moving_avg_data_y = get_last_node(moving_avg_data_y);
    IntNode* last_moving_avg_data_z = get_last_node(moving_avg_data_z);
    
    difference_data_x = add_new_difference(
      difference_data_x,
      last_moving_avg_data_x->prev->data,
      last_moving_avg_data_x->data);
    difference_data_y = add_new_difference(
      difference_data_y,
      last_moving_avg_data_y->prev->data,
      last_moving_avg_data_y->data);
    difference_data_z = add_new_difference(
      difference_data_z,
      last_moving_avg_data_z->prev->data,
      last_moving_avg_data_z->data);
    
    last_accel_x = new_accel_x;
    last_accel_y = new_accel_y;
    last_accel_z = new_accel_z;
  }
  
  static char infoMsg[32];
  snprintf(infoMsg, sizeof(infoMsg),
           "Difference: %d, %d, %d",
          get_last_node(difference_data_x)->data,
          get_last_node(difference_data_y)->data,
          get_last_node(difference_data_z)->data);
  text_layer_set_text(s_debug_layer, infoMsg);
}

IntNode* add_new_moving_avg(IntNode* stream, int16_t prev_data, int16_t new_data)
{
  int16_t new_moving_avg = (int16_t)((prev_data + new_data) / 2);
  return add_to_int_linked_list(stream, new_moving_avg);
}

IntNode* add_new_difference(IntNode* stream, int16_t prev_data, int16_t new_data)
{
  int16_t new_difference = new_data - prev_data;
  return add_to_int_linked_list(stream, new_difference);
}
#pragma once

#define FALSE 0
#define TRUE 1

#define SIZE_OF_STREAM 50
#define NUM_SAMPLES 25
#define GESTURE_LENGTH_THRESHOLD 25

#define abs(x) (x >= 0 ? x : -x)
#define sgn(x) (x >= 0 ? 1 : -1)
#define DIFFERENCE_THRESHOLD_X 200
#define DIFFERENCE_THRESHOLD_Y 200
#define DIFFERENCE_THRESHOLD_Z 200

static uint gesture_mode = FALSE;

struct IntNode
{
  int16_t data;
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
IntNode* get_last_node(IntNode*);
IntNode* add_to_int_linked_list(IntNode*, int16_t);

static IntNode* accel_data_x;
static IntNode* accel_data_y;
static IntNode* accel_data_z;

static IntNode* moving_avg_data_x;
static IntNode* moving_avg_data_y;
static IntNode* moving_avg_data_z;

static IntNode* difference_data_x;
static IntNode* difference_data_y;
static IntNode* difference_data_z;

static IntNode* last_large_change_x;
static IntNode* last_large_change_y;
static IntNode* last_large_change_z;

void init_data_streams(uint);
void destroy_data_streams();

void add_accel_data_to_streams(AccelData*, uint, TextLayer*);
IntNode* add_new_moving_avg(IntNode*, int16_t, int16_t);
IntNode* add_new_difference(IntNode*, int16_t, int16_t);
IntNode* add_new_last_large_change(IntNode*, int16_t, int16_t, int16_t, IntNode*);

void send_next_request();
void send_prev_request();
void check_to_send_next_request();
void check_to_send_prev_request();

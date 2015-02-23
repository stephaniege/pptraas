#include <pebble.h>
#include "gesture.h"

#define KEY_PAIRING_CODE_DIGIT1 0
#define KEY_PAIRING_CODE_DIGIT2 1
#define KEY_PAIRING_CODE_DIGIT3 2
#define KEY_PAIRING_CODE_DIGIT4 3
#define KEY_PAIRING_STATUS 4
#define KEY_NEXT 5
#define KEY_NEXT_STATUS 6
#define KEY_PREV 7
#define KEY_PREV_STATUS 8

static Window *s_code_window;
static TextLayer *s_enter_code_layer;
static TextLayer *s_code_digit_layers[4];

static uint s_code_digits[4];
static char *s_numerals[10] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static uint current_digit_pos;

static uint get_next_numeral_pos(uint i)
{
  return (i + 1) % 10;
}

static uint get_prev_numeral_pos(uint i)
{
  return (i == 0) ? 9 : (i - 1);  
}

static void init_digit_layers(Window *window)
{
  TextLayer *code_digit_layer;
  for (uint i = 0; i < sizeof(s_code_digit_layers) / sizeof(TextLayer*); i++)
  {
    s_code_digits[i] = 0;
    
    // Create code digit TextLayer.
    s_code_digit_layers[i] = text_layer_create(GRect((32 * i) + 8, 55, 32, 50));
    code_digit_layer = s_code_digit_layers[i];
    text_layer_set_background_color(code_digit_layer, GColorBlack);
    text_layer_set_text_color(code_digit_layer, GColorWhite);
    text_layer_set_overflow_mode(code_digit_layer, GTextOverflowModeWordWrap);
    text_layer_set_text(code_digit_layer, s_numerals[s_code_digits[i]]);

    text_layer_set_font(code_digit_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
    text_layer_set_text_alignment(code_digit_layer, GTextAlignmentCenter);
    
    // Add it as a child layer to the Window's root layer.
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_code_digit_layers[i]));
  }
}

static void init_text_layers_cw(Window *window)
{
  // Add text layer that instructs the user to enter a code.
  s_enter_code_layer = text_layer_create(GRect(0, 20, 145, 30));
  text_layer_set_text_color(s_enter_code_layer, GColorBlack);
  text_layer_set_text(s_enter_code_layer, "Enter your remote code");
  text_layer_set_overflow_mode(s_enter_code_layer, GTextOverflowModeWordWrap);

  text_layer_set_font(s_enter_code_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(s_enter_code_layer, GTextAlignmentCenter);
    
  // Add it as a child layer to the Window's root layer.
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_enter_code_layer));
}

static void destroy_digit_layers(Window *window)
{
  for (uint i = 0; i < sizeof(s_code_digit_layers) / sizeof(TextLayer*); i++)
  {
    text_layer_destroy(s_code_digit_layers[i]);
  }
}

static void destroy_text_layers_cw(Window *window)
{
  text_layer_destroy(s_enter_code_layer);
}

static void make_text_layer_black(TextLayer *text_layer)
{
  text_layer_set_background_color(text_layer, GColorBlack);
  text_layer_set_text_color(text_layer, GColorWhite);
}

static void make_text_layer_white(TextLayer *text_layer)
{
  text_layer_set_background_color(text_layer, GColorWhite);
  text_layer_set_text_color(text_layer, GColorBlack);
}

static void select_digit_layer(uint pos)
{
  for (uint i = 0; i < sizeof(s_code_digit_layers) / sizeof(TextLayer*); i++)
  {
    if (i != pos)
    {
      // If this is not the selected digit, set its background to white
      // and the color of the text to black.
      make_text_layer_white(s_code_digit_layers[i]);
    }
    else
     {
      // Otherwise, set its background to black and the color of the text to white.
      make_text_layer_black(s_code_digit_layers[i]);
     }
  }
}

static void send_code()
{
  // Begin dictionary.
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  // Add key-value pairs for each of the digits.
  dict_write_uint8(iter, KEY_PAIRING_CODE_DIGIT4, s_code_digits[3]);
  dict_write_uint8(iter, KEY_PAIRING_CODE_DIGIT3, s_code_digits[2]);
  dict_write_uint8(iter, KEY_PAIRING_CODE_DIGIT2, s_code_digits[1]);
  dict_write_uint8(iter, KEY_PAIRING_CODE_DIGIT1, s_code_digits[0]);

  // Send the message!
  app_message_outbox_send();
  APP_LOG(APP_LOG_LEVEL_INFO, "Sent the pairing code!");
}

static void up_click_handler_cw(ClickRecognizerRef recognizer, void *context)
{
  s_code_digits[current_digit_pos] = get_next_numeral_pos(s_code_digits[current_digit_pos]);
  text_layer_set_text(
    s_code_digit_layers[current_digit_pos],
    s_numerals[s_code_digits[current_digit_pos]]);
}

static void down_click_handler_cw(ClickRecognizerRef recognizer, void *context)
{
  s_code_digits[current_digit_pos] = get_prev_numeral_pos(s_code_digits[current_digit_pos]);
  text_layer_set_text(
    s_code_digit_layers[current_digit_pos],
    s_numerals[s_code_digits[current_digit_pos]]);
}

static void select_click_handler_cw(ClickRecognizerRef recognizer, void *context)
{
  if (current_digit_pos != 3)
  {
    current_digit_pos++;
    select_digit_layer(current_digit_pos);
  }
  else
  {
    // If the last digit was being edited, send the code.
    send_code();
  }
}

static void back_click_handler_cw(ClickRecognizerRef recognizer, void *context)
{
  if (current_digit_pos != 0)
  {
    current_digit_pos--;
    select_digit_layer(current_digit_pos);
  }
  else
  {
    // If the first digit was being edited, close the app.
    window_stack_pop_all(true);
  }
}

static void click_config_provider_cw(void *context)
{
  // Register the ClickHandlers.
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler_cw);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler_cw);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler_cw);
  window_single_click_subscribe(BUTTON_ID_BACK, back_click_handler_cw);
}

static void code_window_load(Window *window)
{
  init_digit_layers(window);
  init_text_layers_cw(window);
  current_digit_pos = 0;
  select_digit_layer(current_digit_pos);
}

static void code_window_unload(Window *window)
{
  destroy_digit_layers(window);
  destroy_text_layers_cw(window);
}

static Window *s_navigation_window;
static GBitmap *s_next_bitmap;
static GBitmap *s_prev_bitmap;
static ActionBarLayer *s_actionbar_layer;
static TextLayer *s_status_layer;
static TextLayer *s_readings_layer;

static void send_next_request()
{
  // Begin dictionary.
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  // Add a key-value pair.
  dict_write_uint8(iter, KEY_NEXT, 0);

  // Send the message!
  app_message_outbox_send();
  APP_LOG(APP_LOG_LEVEL_INFO, "Sent a request to go to the next slide!");
}

static void up_click_handler_nw(ClickRecognizerRef recognizer, void *context)
{
  send_next_request();
}

static void send_prev_request()
{
  // Begin dictionary.
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  // Add a key-value pair.
  dict_write_uint8(iter, KEY_PREV, 0);

  // Send the message!
  app_message_outbox_send();
  APP_LOG(APP_LOG_LEVEL_INFO, "Sent a request to go to the previous slide!");
}

static void down_click_handler_nw(ClickRecognizerRef recognizer, void *context)
{
  send_prev_request();
}

static void select_click_handler_nw(ClickRecognizerRef recognizer, void *context)
{
  // Toggle the gesture recognition mode.
  gesture_mode = gesture_mode ? FALSE : TRUE;
  if (gesture_mode) { text_layer_set_text(s_status_layer, "Gesture mode ON."); }
  else { text_layer_set_text(s_status_layer, "Gesture mode OFF."); }
}

static void click_config_provider_nw(void *context)
{
  // Register the ClickHandlers.
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler_nw);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler_nw);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler_nw);
}

static void init_button_layers(Window *window)
{
  s_prev_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PREV_SMALL_SCALED);
  s_next_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NEXT_SMALL_SCALED);
  
  s_actionbar_layer = action_bar_layer_create();
  action_bar_layer_add_to_window(s_actionbar_layer, window);
  action_bar_layer_set_background_color(s_actionbar_layer, GColorBlack);
  action_bar_layer_set_icon(s_actionbar_layer, BUTTON_ID_UP, s_next_bitmap);
  action_bar_layer_set_icon(s_actionbar_layer, BUTTON_ID_DOWN, s_prev_bitmap);
  layer_add_child(window_get_root_layer(window), (Layer *)s_actionbar_layer);
  
  // Add configuration provider to the action bar.
  action_bar_layer_set_click_config_provider(s_actionbar_layer, click_config_provider_nw);
}

static void init_text_layers_nw(Window *window)
{
  s_status_layer = text_layer_create(GRect(0, 20, 120, 100));
  text_layer_set_text_color(s_status_layer, GColorBlack);
  text_layer_set_text(s_status_layer, "No request sent.");
  text_layer_set_overflow_mode(s_status_layer, GTextOverflowModeWordWrap);

  text_layer_set_font(s_status_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(s_status_layer, GTextAlignmentCenter);
    
  // Add it as a child layer to the Window's root layer.
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_status_layer));
  
  s_readings_layer = text_layer_create(GRect(0, 50, 120, 100));
  text_layer_set_text_color(s_readings_layer, GColorBlack);
  text_layer_set_overflow_mode(s_readings_layer, GTextOverflowModeWordWrap);
  text_layer_set_font(s_readings_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  text_layer_set_text_alignment(s_readings_layer, GTextAlignmentCenter);
    
  // Add it as a child layer to the Window's root layer.
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_readings_layer));
}

static void destroy_button_layers(Window *window)
{
  gbitmap_destroy(s_prev_bitmap);
  gbitmap_destroy(s_next_bitmap);
  action_bar_layer_destroy(s_actionbar_layer);
}

static void destroy_text_layers_nw(Window *window)
{
  text_layer_destroy(s_status_layer);
  text_layer_destroy(s_readings_layer);
}

static void navigation_window_load(Window *window)
{
  init_button_layers(window);
  init_text_layers_nw(window);
}

static void navigation_window_unload(Window *window)
{
  destroy_button_layers(window);
  destroy_text_layers_nw(window);
}

static void inbox_received_callback(DictionaryIterator *iterator, void *context)
{
  APP_LOG(APP_LOG_LEVEL_INFO, "Message received!");
  
  // Read first item from the dictionary.
  Tuple *t = dict_read_first(iterator);

  // For all items...
  while (t != NULL)
  {
    // Which key was received?
    switch(t->key)
    {
      case KEY_PAIRING_STATUS:
        if ((int)t->value->int32)
        {
          APP_LOG(APP_LOG_LEVEL_INFO, "Pairing was successful! :)");
          // If the pairing was successful, remove the code Window.
          window_stack_remove(s_code_window, true);
        }
        else
        {
          APP_LOG(APP_LOG_LEVEL_INFO, "Pairing was unsuccessful! :(");
          // If the pairing was unsuccessful, reset the position of the cursor
          // and reset all numbers to zero.
          for (uint i = 0; i < 4; i++)
          {
            s_code_digits[i] = 0;
            text_layer_set_text(s_code_digit_layers[i], s_numerals[s_code_digits[i]]);  
          }
          current_digit_pos = 0;
          select_digit_layer(current_digit_pos);
        }
        break;
      case KEY_NEXT_STATUS:
        if ((int)t->value->int32)
        {
          APP_LOG(APP_LOG_LEVEL_INFO, "Next slide request was successful! :)");
          text_layer_set_text(s_status_layer, "Showing next slide.");
        }
        else
        {
          APP_LOG(APP_LOG_LEVEL_INFO, "Next slide request was unsuccessful! :(");
          text_layer_set_text(s_status_layer, "Unable to show next slide.");
        }
        break;
      case KEY_PREV_STATUS:
        if ((int)t->value->int32)
        {
          APP_LOG(APP_LOG_LEVEL_INFO, "Previous slide request was successful! :)");
          text_layer_set_text(s_status_layer, "Showing previous slide.");
        }
        else
        {
          APP_LOG(APP_LOG_LEVEL_INFO, "Previous slide request was unsuccessful! :(");
          text_layer_set_text(s_status_layer, "Unable to show previous slide.");
        }
        break;
      default:
        APP_LOG(APP_LOG_LEVEL_ERROR, "Key %d not recognized!", (int)t->key);
        break;
     }

    // Look for next item in the dictionary.
    t = dict_read_next(iterator);
  }
}

static void inbox_dropped_callback(AppMessageResult reason, void *context)
{
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context)
{
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context)
{
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void data_handler(AccelData *data, uint32_t num_samples)
{
  // Add the data to the acceleration streams.
  add_accel_data_to_streams(data, s_readings_layer);
  
  // Long-lived buffer.
  static char s_buffer[128];

  // Compose string of all data
  /* snprintf(s_buffer, sizeof(s_buffer), 
    "N X,Y,Z\n0 %d,%d,%d\n1 %d,%d,%d\n2 %d,%d,%d", 
    data[NUM_SAMPLES - 3].x, data[NUM_SAMPLES - 3].y, data[NUM_SAMPLES - 3].z, 
    data[NUM_SAMPLES - 2].x, data[NUM_SAMPLES - 2].y, data[NUM_SAMPLES - 2].z, 
    data[NUM_SAMPLES - 1].x, data[NUM_SAMPLES - 1].y, data[NUM_SAMPLES - 1].z
  ); */
  // Show the data.
  // text_layer_set_text(s_readings_layer, s_buffer);
}

static void init()
{
  // Register callbacks.
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage.
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  // Create main Window element to enter the code and assign to pointer.
  s_code_window = window_create();

  // Set handlers to manage the elements inside the code Window.
  window_set_window_handlers(s_code_window, (WindowHandlers) {
    .load = code_window_load,
    .unload = code_window_unload
  });
  
  // Add configuration provider to the code Window.
  window_set_click_config_provider(s_code_window, click_config_provider_cw);
  
  // Create main Window element to show the navigation buttons.
  s_navigation_window = window_create();
  
    // Set handlers to manage the elements inside the navigation Window.
  window_set_window_handlers(s_navigation_window, (WindowHandlers) {
    .load = navigation_window_load,
    .unload = navigation_window_unload
  });
  
  // Show the navigation Window on the watch, with `animated` set to `true`.
  window_stack_push(s_navigation_window, true);
  // Show the code Window on the watch, with `animated` set to `true`.
  window_stack_push(s_code_window, true);
  
  // Set up data streams for the accelerometer.
  init_data_streams(SIZE_OF_STREAM);
  
  // Subscribe to the accelerometer data service.
  accel_data_service_subscribe(NUM_SAMPLES, data_handler);
  // Choose update rate.
  accel_service_set_sampling_rate(ACCEL_SAMPLING_25HZ);
  
}

static void deinit()
{
  // Destroy data streams for the accelerometer.
  destroy_data_streams();
  
  // Destroy Window.
  window_destroy(s_code_window);
}

int main(void)
{
  init();
  app_event_loop();
  deinit();
}

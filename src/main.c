#include <pebble.h>

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

static uint get_complete_code()
{
  return ((s_code_digits[0] * 1000) +
          (s_code_digits[1] * 100) +
          (s_code_digits[2] * 10) +
          s_code_digits[3]);
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
  
  // TODO: Remove once implemented.
  get_complete_code();
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
    // If the last digit was selected, remove the code window.
    window_stack_remove(s_code_window, true);
  }
}

static void click_config_provider_cw(void *context)
{
  // Register the ClickHandlers.
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler_cw);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler_cw);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler_cw);
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
static BitmapLayer *s_next_layer;
static GBitmap *s_next_bitmap;
static BitmapLayer *s_prev_layer;
static GBitmap *s_prev_bitmap;

static void init_button_layers(Window *window)
{
  s_prev_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PREV_SMALL);
  s_prev_layer = bitmap_layer_create(GRect(0, 26, 73, 99));
  bitmap_layer_set_bitmap(s_prev_layer, s_prev_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_prev_layer));
  
  s_next_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_NEXT_SMALL);
  s_next_layer = bitmap_layer_create(GRect(73, 26, 73, 99));
  bitmap_layer_set_bitmap(s_next_layer, s_next_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_next_layer));
}

static void destroy_button_layers(Window *window)
{
  bitmap_layer_destroy(s_prev_layer);
  bitmap_layer_destroy(s_next_layer);
}

static void navigation_window_load(Window *window)
{
  init_button_layers(window);
}

static void navigation_window_unload(Window *window)
{
  destroy_button_layers(window);
}

static void init()
{
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
  
  // Add configuration provider to the navigation Window.
  // window_set_click_config_provider(s_code_window, click_config_provider_cw);
  
  // Show the navigation Window on the watch, with `animated` set to `true`.
  window_stack_push(s_navigation_window, true);
  // Show the code Window on the watch, with `animated` set to `true`.
  window_stack_push(s_code_window, true);
}

static void deinit()
{
  // Destroy Window.
  window_destroy(s_code_window);
}

int main(void)
{
  init();
  app_event_loop();
  deinit();
}

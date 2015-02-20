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

static void init_digit_layers(Window *window)
{
  TextLayer *code_digit_layer;
  for (uint i = 0; i < sizeof(s_code_digit_layers) / sizeof(TextLayer*); i++)
  {
    s_code_digits[i] = 0;
    
    // Create code digit TextLayer.
    s_code_digit_layers[i] = text_layer_create(GRect((32 * i) + 11, 55, 32, 50));
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
  
  // Add text layer that instructs the user to enter a code.
  s_enter_code_layer = text_layer_create(GRect(0, 0, 120, 40));
  text_layer_set_text_color(s_enter_code_layer, GColorBlack);
  text_layer_set_text(s_enter_code_layer, "Enter your remote code");
  text_layer_set_overflow_mode(s_enter_code_layer, GTextOverflowModeWordWrap);

  text_layer_set_font(s_enter_code_layer, fonts_get_system_font(FONT_KEY_BITHAM_18_LIGHT_SUBSET));
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

static void click_config_provider_cw(void *context) {
  // Register the ClickHandlers.
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler_cw);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler_cw);
}

static void code_window_load(Window *window) {
  init_digit_layers(window);
  current_digit_pos = 0;
}

static void code_window_unload(Window *window) {
  destroy_digit_layers(window);
}

static void init() {
  // Create main Window element to enter the code and assign to pointer.
  s_code_window = window_create();

  // Set handlers to manage the elements inside the code Window.
  window_set_window_handlers(s_code_window, (WindowHandlers) {
    .load = code_window_load,
    .unload = code_window_unload
  });
  
  // Add configuration provider to the code Window.
  window_set_click_config_provider(s_code_window, click_config_provider_cw);
  
  // Show the code Window on the watch, with `animated` set to `true`.
  window_stack_push(s_code_window, true);
}

static void deinit() {
  // Destroy Window.
  window_destroy(s_code_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}

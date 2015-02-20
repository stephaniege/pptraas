#include "code_input.h"
#include <pebble.h>

// BEGIN AUTO-GENERATED UI CODE; DO NOT MODIFY
static Window *s_window;
static MenuLayer *digit0;

static void initialise_ui(void) {
  s_window = window_create();
  window_set_fullscreen(s_window, false);
  
  // digit0
  digit0 = menu_layer_create(GRect(0, 0, 144, 152));
  menu_layer_set_click_config_onto_window(digit0, s_window);
  layer_add_child(window_get_root_layer(s_window), (Layer *)digit0);
}

static void destroy_ui(void) {
  window_destroy(s_window);
  menu_layer_destroy(digit0);
}
// END AUTO-GENERATED UI CODE

static void handle_window_load(Window* window) {
  initialise_ui();
}

static void handle_window_unload(Window* window) {
  destroy_ui();
}

void show_code_input(void) {
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = handle_window_load,
    .unload = handle_window_unload,
  });
  window_stack_push(s_window, true);
}

void hide_code_input(void) {
  window_stack_remove(s_window, true);
}

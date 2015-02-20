#include <pebble.h>

static Window *s_window;
static SimpleMenuLayer *s_digit0;
static SimpleMenuItem s_number_items[10];
static SimpleMenuSection s_menu_sections[1];
static char *s_numerals[10] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static void init_number_items(SimpleMenuItem *number_items) {
  for (int i = 0; i < 10; i++) {
    number_items[i] = (SimpleMenuItem) {
      .title = s_numerals[i]
    };
  }
}

static void init_menu_sections() {
  s_menu_sections[0] = (SimpleMenuSection)
  {
    // .title = "Fruits",
    .items = s_number_items,
    .num_items = 10
  };  
}

static void main_window_load(Window *window) {
  // Create leftmost digit, `digit0`.
  init_number_items(s_number_items);
  init_menu_sections();
  s_digit0 = simple_menu_layer_create(
    GRect(0, 0, 144, 152),
    window,
    s_menu_sections,
    10,
    NULL
  );
  menu_layer_set_click_config_onto_window((MenuLayer *)s_digit0, window);
  layer_add_child(
    window_get_root_layer(window),
    (Layer *)(s_digit0));
    // (Layer *)digit0);
}

static void main_window_unload(Window *window) {
  // Destroy digit0.
  simple_menu_layer_destroy(s_digit0);
}

static void init() {
  // Create main Window element and assign to pointer.
  s_window = window_create();

  // Set handlers to manage the elements inside the Window.
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with `animated` set to `true`.
  window_stack_push(s_window, true);
}

static void deinit() {
  // Destroy Window.
  window_destroy(s_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"


#define MY_UUID { 0x3F, 0x88, 0xA0, 0xD5, 0x83, 0x1E, 0x4F, 0x15, 0x87, 0x42, 0x13, 0xBC, 0x42, 0x53, 0x69, 0x0C }
PBL_APP_INFO(MY_UUID,
             "Trekkie", "Zach Bruggeman",
             1, 0, /* App version */
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;
BmpContainer background_image;

TextLayer text_date_layer;
TextLayer text_time_layer;
TextLayer text_unix_layer;

void update_display(PblTm *tick_time) {
  static char date_text[] = "Xxx 00";
  static char new_date_text[] = "Xxx 00";
  static char time_text[] = "00:00";
  char *time_format;

  // Draws only if changed, saves your precious battery!
  string_format_time(new_date_text, sizeof(date_text), "%a %e", tick_time);
  if (strncmp(new_date_text, date_text, sizeof(date_text)) != 0) {
      strncpy(date_text, new_date_text, sizeof(date_text));
      text_layer_set_text(&text_date_layer, date_text);
  }

  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }

  string_format_time(time_text, sizeof(time_text), time_format, tick_time);

  text_layer_set_text(&text_time_layer, time_text);

}

void handle_minute_tick(AppContextRef ctx, PebbleTickEvent *t) {
  (void)ctx;

  update_display(t->tick_time);
}

void handle_init(AppContextRef ctx) {
  (void)ctx;

  window_init(&window, "Trekkie");
  window_stack_push(&window, true /* Animated */);

  resource_init_current_app(&APP_RESOURCES);

  bmp_init_container(RESOURCE_ID_IMAGE_BACKGROUND, &background_image);
  layer_add_child(&window.layer, &background_image.layer.layer);

  text_layer_init(&text_date_layer, window.layer.frame);
  text_layer_set_text_color(&text_date_layer, GColorWhite);
  text_layer_set_background_color(&text_date_layer, GColorClear);
  layer_set_frame(&text_date_layer.layer, GRect(92, 5, 144-92, 168-5));
  text_layer_set_font(&text_date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LCARS_BOLD_17)));
  layer_add_child(&window.layer, &text_date_layer.layer);

  text_layer_init(&text_time_layer, window.layer.frame);
  text_layer_set_text_color(&text_time_layer, GColorWhite);
  text_layer_set_background_color(&text_time_layer, GColorClear);
  layer_set_frame(&text_time_layer.layer, GRect(40, 48, 144-40, 168-48));
  text_layer_set_font(&text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LCARS_60)));
  layer_add_child(&window.layer, &text_time_layer.layer);

  // No blank screen on init.
  PblTm tick_time;
  get_time(&tick_time);
  update_display(&tick_time);

}

void handle_deinit(AppContextRef ctx) {
  (void)ctx;

  bmp_deinit_container(&background_image);

}

void pbl_main(void *params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .deinit_handler = &handle_deinit,

    .tick_info = {
      .tick_handler = &handle_minute_tick,
      .tick_units = MINUTE_UNIT
    }
  };
  app_event_loop(params, &handlers);
}

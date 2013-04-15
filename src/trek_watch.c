#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#define MY_UUID { 0x3F, 0x88, 0xA0, 0xD5, 0x83, 0x1E, 0x4F, 0x15, 0x87, 0x42, 0x13, 0xBC, 0x42, 0x53, 0x69, 0x0C }
PBL_APP_INFO(MY_UUID,
             "Trekkie", "Zach Bruggeman",
             2, 0, /* App version */
             RESOURCE_ID_IMAGE_MENU_ICON,
             APP_INFO_WATCH_FACE);

Window window;
BmpContainer background_image;

TextLayer text_date_layer;
TextLayer text_nice_date_layer;
TextLayer text_time_layer;
TextLayer text_ampm_layer;
TextLayer text_stardate_layer;

void update_display(PblTm *tick_time) {
  static char date_text[] = "00/00/00";
  static char new_date_text[] = "00/00/00";
  static char nice_date_text[] = "Xxx. Xxx. 00";
  static char time_text[] = "00:00";
  static char ampm_text[] = "xx";
  static char stardate_text[] = "0000.000";
  char *time_format;

  // Date layers — These only need to update once a day, so this checks if it's different, and updates it only if it's different.
  string_format_time(new_date_text, sizeof(date_text), "%x", tick_time);
  string_format_time(nice_date_text, sizeof(nice_date_text), "%a.%n%b.%n%d", tick_time);
  string_format_time(stardate_text, sizeof(stardate_text), "%Y.%j", tick_time); // Based on Star Trek 2009 film's Stardate format
  if (strncmp(new_date_text, date_text, sizeof(date_text)) != 0) {
      strncpy(date_text, new_date_text, sizeof(date_text));
      text_layer_set_text(&text_date_layer, date_text);
      text_layer_set_text(&text_nice_date_layer, nice_date_text);
      text_layer_set_text(&text_stardate_layer, stardate_text);  
  }

  // Time layer
  if (clock_is_24h_style()) {
    time_format = "%R";
  } else {
    time_format = "%I:%M";
  }
  string_format_time(time_text, sizeof(time_text), time_format, tick_time);
  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }
  text_layer_set_text(&text_time_layer, time_text);

  // AM/PM layer
  if (!clock_is_24h_style()) {
    string_format_time(ampm_text, sizeof(ampm_text), "%p", tick_time);
    if (ampm_text[0] == 'A') {
      strncpy(ampm_text, "am", sizeof("am"));
    } else {
      strncpy(ampm_text, "pm", sizeof("pm"));
    }
  }
  text_layer_set_text(&text_ampm_layer, ampm_text);
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

  // Date Layer
  text_layer_init(&text_date_layer, window.layer.frame);
  text_layer_set_text_color(&text_date_layer, GColorWhite);
  text_layer_set_background_color(&text_date_layer, GColorClear);
  layer_set_frame(&text_date_layer.layer, GRect(79, 5, 144-79, 168-5));
  text_layer_set_font(&text_date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LCARS_BOLD_17)));
  layer_add_child(&window.layer, &text_date_layer.layer);

  // Nice date Layer
  text_layer_init(&text_nice_date_layer, window.layer.frame);
  text_layer_set_text_color(&text_nice_date_layer, GColorBlack);
  text_layer_set_background_color(&text_nice_date_layer, GColorClear);
  layer_set_frame(&text_nice_date_layer.layer, GRect(6, 26, 144-6, 168-26));
  text_layer_set_font(&text_nice_date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LCARS_BOLD_17)));
  layer_add_child(&window.layer, &text_nice_date_layer.layer);

  // Time Layer
  text_layer_init(&text_time_layer, window.layer.frame);
  text_layer_set_text_color(&text_time_layer, GColorWhite);
  text_layer_set_background_color(&text_time_layer, GColorClear);
  layer_set_frame(&text_time_layer.layer, GRect(36, 12, 144-36, 168-12));
  text_layer_set_font(&text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LCARS_60)));
  layer_add_child(&window.layer, &text_time_layer.layer);

  // AMPM Layer
  text_layer_init(&text_ampm_layer, window.layer.frame);
  text_layer_set_text_color(&text_ampm_layer, GColorWhite);
  text_layer_set_background_color(&text_ampm_layer, GColorClear);
  layer_set_frame(&text_ampm_layer.layer, GRect(36, 69, 144-36, 168-69));
  text_layer_set_font(&text_ampm_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LCARS_BOLD_17)));
  layer_add_child(&window.layer, &text_ampm_layer.layer);

  // Stardate Layer
  text_layer_init(&text_stardate_layer, window.layer.frame);
  text_layer_set_text_color(&text_stardate_layer, GColorWhite);
  text_layer_set_background_color(&text_stardate_layer, GColorClear);
  layer_set_frame(&text_stardate_layer.layer, GRect(36, 95, 144-36, 168-95));
  text_layer_set_font(&text_stardate_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LCARS_36)));
  layer_add_child(&window.layer, &text_stardate_layer.layer);

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

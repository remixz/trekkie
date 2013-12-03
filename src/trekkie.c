#include <pebble.h>

static Window *window;
static TextLayer *text_date_layer;
static TextLayer *text_nice_date_layer;
static TextLayer *text_time_layer;
static TextLayer *text_ampm_layer;
static TextLayer *text_stardate_layer;
static GBitmap *background_image;
static BitmapLayer *background_image_layer;
static GBitmap *battery_charge_image;
static BitmapLayer *battery_charge_image_layer;
static TextLayer *battery_percent;
static BatteryChargeState old_charge_state;
static Layer *battery_status_layer;
static TextLayer *bluetooth_status_layer;

void update_battery_display(BatteryChargeState charge_state){
  old_charge_state=charge_state;
  char* battery_percent_text="000";
  snprintf(battery_percent_text, sizeof(battery_percent_text), "%02u", charge_state.charge_percent);
  if(charge_state.charge_percent!=100) battery_percent_text[2]='%';
  text_layer_set_text(battery_percent, battery_percent_text);
  layer_set_hidden(bitmap_layer_get_layer(battery_charge_image_layer), !charge_state.is_charging);
  layer_mark_dirty(battery_status_layer);
}

void update_bluetooth_status(bool connected){
  text_layer_set_text(bluetooth_status_layer, connected?"O":"X");
}

void battery_status_layer_update(Layer* layer, GContext* ctx){
  if(old_charge_state.is_plugged){
    graphics_fill_rect(ctx, GRect(7, 93, 2, 2), 0, 0);
  }
  else{
    graphics_fill_rect(ctx, GRect(4, 0, old_charge_state.charge_percent*16/100, 3), 0, 0);
  }
}

void update_display(struct tm* tick_time) {
  static char date_text[] = "00.00.00";
  static char new_date_text[] = "00.00.00";
  static char nice_date_text[] = "Xxx. Xxx. 00";
  static char time_text[] = "00:00";
  static char ampm_text[] = "  ";
  static char stardate_text[] = "0000.000";
  static char *time_format=0;
  time_format=clock_is_24h_style()?"%R":"%I:%M";

  // Date layers — These only need to update once a day, so this checks if it's different, and updates it only if it's different.
  strftime(new_date_text, sizeof(date_text), "%d.%m.%y", tick_time);
  strftime(nice_date_text, sizeof(nice_date_text), "%a.%n%b.%n%d", tick_time);
  strftime(stardate_text, sizeof(stardate_text), "%Y.%j", tick_time); // Based on Star Trek 2009 film's Stardate format
  if (strncmp(new_date_text, date_text, sizeof(date_text)) != 0) {
      strncpy(date_text, new_date_text, sizeof(date_text));
      text_layer_set_text(text_date_layer, date_text);
      text_layer_set_text(text_nice_date_layer, nice_date_text);
      text_layer_set_text(text_stardate_layer, stardate_text);  
  }

  // Time layer
  strftime(time_text, sizeof(time_text), time_format, tick_time);
  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }
  text_layer_set_text(text_time_layer, time_text);

  // AM/PM layer
  if (!clock_is_24h_style()) {
    strftime(ampm_text, sizeof(ampm_text), "%p", tick_time);
    if (ampm_text[0] == 'A') {
      strncpy(ampm_text, "am", sizeof("am"));
    } else {
      strncpy(ampm_text, "pm", sizeof("pm"));
    }
  }
  text_layer_set_text(text_ampm_layer, ampm_text);
}

void handle_minute_tick(struct tm* tick_time, TimeUnits units_changed) {
  update_display(tick_time);
}

static void init(void) {
  window=window_create();
  window_stack_push(window, true /* Animated */);
  
  background_image_layer=bitmap_layer_create(layer_get_frame(window_get_root_layer(window)));
  background_image=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
  bitmap_layer_set_bitmap(background_image_layer, background_image);
  
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(background_image_layer));

  // Date Layer
  text_date_layer=text_layer_create(GRect(79, 5, 144-79, 168-5));
  text_layer_set_text_color(text_date_layer, GColorWhite);
  text_layer_set_background_color(text_date_layer, GColorClear);
  text_layer_set_font(text_date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LCARS_BOLD_17)));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_date_layer));

  // Nice date Layer
  text_nice_date_layer=text_layer_create(GRect(6, 26, 144-6, 168-26));
  text_layer_set_background_color(text_nice_date_layer, GColorClear);
  text_layer_set_font(text_nice_date_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LCARS_BOLD_17)));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_nice_date_layer));

  // Time Layer
  text_time_layer=text_layer_create(GRect(36, 12, 144-36, 168-12));
  text_layer_set_text_color(text_time_layer, GColorWhite);
  text_layer_set_background_color(text_time_layer, GColorClear);
  text_layer_set_font(text_time_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LCARS_60)));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_time_layer));

  // AMPM Layer
  text_ampm_layer=text_layer_create(GRect(36, 69, 144-36, 168-69));
  text_layer_set_text_color(text_ampm_layer, GColorWhite);
  text_layer_set_background_color(text_ampm_layer, GColorClear);
  text_layer_set_font(text_ampm_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LCARS_BOLD_17)));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_ampm_layer));

  // Stardate Layer
  text_stardate_layer=text_layer_create(GRect(36, 95, 144-36, 168-95));
  text_layer_set_text_color(text_stardate_layer, GColorWhite);
  text_layer_set_background_color(text_stardate_layer, GColorClear);
  text_layer_set_font(text_stardate_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LCARS_36)));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(text_stardate_layer));

  //Battery  layer
  battery_percent=text_layer_create(GRect(8, 93, 27, 115));
  text_layer_set_background_color(battery_percent, GColorClear);
  text_layer_set_font(battery_percent, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LCARS_BOLD_17)));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(battery_percent));
  
  //Battery charging layer
  battery_charge_image_layer=bitmap_layer_create(GRect(10, 92, 16, 3));
  battery_charge_image=gbitmap_create_with_resource(RESOURCE_ID_BATTERY_CHARGING_IMAGE);
  bitmap_layer_set_bitmap(battery_charge_image_layer, battery_charge_image);
  layer_set_hidden(bitmap_layer_get_layer(battery_charge_image_layer), true);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(battery_charge_image_layer));
  
  //Battery status layer
  battery_status_layer=layer_create(GRect(6, 92, 20, 3));
  layer_add_child(window_get_root_layer(window), battery_status_layer);
  layer_set_update_proc(battery_status_layer, battery_status_layer_update);
  
  //Bluetooth status layer
  bluetooth_status_layer=text_layer_create(GRect(15, 107, 14, 14));
  text_layer_set_background_color(bluetooth_status_layer, GColorClear);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(bluetooth_status_layer));

  //Prevent blank screen on init.
  time_t now=time(NULL);
  struct tm* tick_time;
  tick_time=localtime(&now);
  update_display(tick_time);
  update_battery_display(battery_state_service_peek());
  update_bluetooth_status(bluetooth_connection_service_peek());

  bluetooth_connection_service_subscribe(update_bluetooth_status);
  battery_state_service_subscribe(update_battery_display);
  tick_timer_service_subscribe(MINUTE_UNIT, &handle_minute_tick);
}

static void deinit(void) {
  gbitmap_destroy(background_image);
  gbitmap_destroy(battery_charge_image);
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}

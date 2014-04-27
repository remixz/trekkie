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
  if(charge_state.charge_percent==100){
    snprintf(battery_percent_text, sizeof(battery_percent_text), "%03u", charge_state.charge_percent);
  }
  else{
    snprintf(battery_percent_text, sizeof(battery_percent_text), "%02u", charge_state.charge_percent);
    battery_percent_text[2]='%';
  }
  text_layer_set_text(battery_percent, battery_percent_text);
  layer_set_hidden(bitmap_layer_get_layer(battery_charge_image_layer), !charge_state.is_charging);
  layer_mark_dirty(battery_status_layer);
}

void update_bluetooth_status(bool connected){
  text_layer_set_text(bluetooth_status_layer, connected?"O":"X");
}

void battery_status_layer_update(Layer* layer, GContext* ctx){
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(0, 0, old_charge_state.charge_percent*19/100, 3), 0, 0);
}

void date_update(struct tm* tick_time, TimeUnits units_changed){
  static char date_text[] = "00.00.00";
  static char nice_date_text[] = "Xxx. Xxx. 00";
  static char stardate_text[] = "0000.000";

  // Date layers
  strftime(date_text, sizeof(date_text), "%d.%m.%y", tick_time);
  strftime(nice_date_text, sizeof(nice_date_text), "%a.%n%b.%n%d", tick_time);
  strftime(stardate_text, sizeof(stardate_text), "%Y.%j", tick_time); // Based on Star Trek 2009 film's Stardate format
  text_layer_set_text(text_date_layer, date_text);
  text_layer_set_text(text_nice_date_layer, nice_date_text);
  text_layer_set_text(text_stardate_layer, stardate_text);
}

void time_update(struct tm* tick_time, TimeUnits units_changed) {
  static char time_text[] = "00:00";
  static char ampm_text[] = " ";
  static char *time_format=0;
  time_format=clock_is_24h_style()?"%R":"%I:%M";

  // Time layer
  strftime(time_text, sizeof(time_text), time_format, tick_time);
  if (!clock_is_24h_style() && (time_text[0] == '0')) {
    memmove(time_text, &time_text[1], sizeof(time_text) - 1);
  }
  text_layer_set_text(text_time_layer, time_text);

  // AM/PM layer
  if (!clock_is_24h_style()) {
    strftime(ampm_text, sizeof(ampm_text), "%p", tick_time);
    text_layer_set_text(text_ampm_layer, (ampm_text[0] == 'A')?"am":"pm");
  }
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

  //Battery percent layer
  battery_percent=text_layer_create(GRect(8, 93, 27, 115));
  text_layer_set_background_color(battery_percent, GColorClear);
  text_layer_set_font(battery_percent, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LCARS_BOLD_17)));
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(battery_percent));
  
  //Battery charging layer
  battery_charge_image_layer=bitmap_layer_create(GRect(8, 92, 19, 3));
  battery_charge_image=gbitmap_create_with_resource(RESOURCE_ID_BATTERY_CHARGING_IMAGE);
  bitmap_layer_set_bitmap(battery_charge_image_layer, battery_charge_image);
  layer_set_hidden(bitmap_layer_get_layer(battery_charge_image_layer), true);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(battery_charge_image_layer));

  //Battery status layer
  battery_status_layer=layer_create(GRect(8, 92, 19, 3));
  layer_add_child(window_get_root_layer(window), battery_status_layer);
  layer_set_update_proc(battery_status_layer, battery_status_layer_update);

  //Bluetooth status layer
  bluetooth_status_layer=text_layer_create(GRect(14, 109, 14, 14));
  text_layer_set_background_color(bluetooth_status_layer, GColorClear);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(bluetooth_status_layer));

  //Prevent blank screen on init.
  time_t now=time(NULL);
  struct tm* tick_time;
  tick_time=localtime(&now);
  time_update(tick_time, 3);
  date_update(tick_time, 3);
  update_battery_display(battery_state_service_peek());
  update_bluetooth_status(bluetooth_connection_service_peek());

  bluetooth_connection_service_subscribe(update_bluetooth_status);
  battery_state_service_subscribe(update_battery_display);
  tick_timer_service_subscribe(MINUTE_UNIT, &time_update);
  tick_timer_service_subscribe(DAY_UNIT, &date_update);
}

static void deinit(void) {
  //destroy layers (reverse order from creation)
  text_layer_destroy(bluetooth_status_layer);
  bitmap_layer_destroy(battery_charge_image_layer);
  text_layer_destroy(battery_percent);
  text_layer_destroy(text_stardate_layer);
  text_layer_destroy(text_ampm_layer);
  text_layer_destroy(text_time_layer);
  text_layer_destroy(text_nice_date_layer);
  text_layer_destroy(text_date_layer);
  bitmap_layer_destroy(background_image_layer);

  //destroy bitmaps
  gbitmap_destroy(background_image);
  gbitmap_destroy(battery_charge_image);
  
  window_destroy(window);
}

int main(void) {
  init();

  //APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}

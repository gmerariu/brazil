
#include <pebble.h>
#include "common.h"


static Window *window;
TextLayer *text_date_layer;
TextLayer *text_time_layer;
TextLayer *text_temp_layer; 

TextLayer *text_unit_layer;
static char *unit_layer="C";
static bool c_or_f;
static char *str_temp;
static char *str_temp_F;
static char *str_stock = " ";
static char *str_stock_change = " ";

TextLayer *text_day_layer;
TextLayer *text_minute_layer;
TextLayer *text_wkday_layer;
TextLayer *battery_level_layer;

TextLayer *text_stock_layer;

GFont *font49;
GFont *font39;
GFont *font21;
GFont *font21B;
GFont *font19;
GFont *font16;
GFont *font10;
GFont *font12;

static GBitmap *image = NULL;
GBitmap *image_BMW = NULL;
static GBitmap *image_BT = NULL;
static GBitmap *image_error = NULL;
static GBitmap *icon_battery_normal;
static GBitmap *icon_battery_charge; 
void set_battery(uint8_t state, int8_t level);
static GBitmap *icon_status_1;
static GBitmap *icon_status_2;

static Layer *p_battery_layer;
static int g_status_display = 0;
static Layer *status_layer;

static BitmapLayer *image_layer;
static BitmapLayer *image_layer_BMW;
static BitmapLayer *image_layer_BT;

static uint8_t battery_level;
static bool battery_plugged;
static char battery_level_string[5];

//Layer *window_layer;

//static bool Bluetooth_on_off;

#define NUMBER_OF_IMAGES 11
const int IMAGE_RESOURCE_IDS[NUMBER_OF_IMAGES] = {
  RESOURCE_ID_CLEAR_DAY,
  RESOURCE_ID_CLEAR_NIGHT,
  RESOURCE_ID_CLOUDY,
  RESOURCE_ID_FOG,
  RESOURCE_ID_PARTLY_CLOUDY_DAY,
  RESOURCE_ID_PARTLY_CLOUDY_NIGHT,
  RESOURCE_ID_RAIN,
  RESOURCE_ID_SLEET,
  RESOURCE_ID_SNOW,
  RESOURCE_ID_WIND,
  RESOURCE_ID_ERROR
    
};

enum {
  WEATHER_TEMPERATURE_F,
  WEATHER_TEMPERATURE_C,
  WEATHER_ICON,
  WEATHER_ERROR,
  LOCATION,
};

static void disp_update(void){
  
  Layer *window_layer = window_get_root_layer(window);
  
      
  
  if (c_or_f == false){
      c_or_f = true;
      //unit_layer="C";
      //image_BMW = gbitmap_create_with_resource(RESOURCE_ID_brazil_flag);
      //image_layer_BMW = bitmap_layer_create(GRect(0, 0, 144, 168));
      //bitmap_layer_set_bitmap(image_layer_BMW, image_BMW);
      //layer_add_child(window_layer, bitmap_layer_get_layer(image_layer_BMW));
      //text_layer_set_text(text_stock_layer, str_stock);
    } else {
      c_or_f = false;
      //image_BMW = gbitmap_create_with_resource(RESOURCE_ID_BMW_M);
      //image_layer_BMW = bitmap_layer_create(GRect(0, 0, 144, 168));
      //bitmap_layer_set_bitmap(image_layer_BMW, image_BMW);
      //layer_add_child(window_layer, bitmap_layer_get_layer(image_layer_BMW));  
      //unit_layer="F";
      //text_layer_set_text(text_unit_layer, unit_layer);
      //text_layer_set_text(text_temp_layer, str_temp_F);
          }
  
      text_layer_set_text(text_unit_layer, unit_layer);
      text_layer_set_text(text_temp_layer, str_temp);
      text_layer_set_text(text_stock_layer, str_stock);
  
  
}

void in_received_handler(DictionaryIterator *received, void *context) {
  // incoming message received
  
  
  // Celsius or Fahrenheit?
  
  Tuple *temperature = dict_find(received, WEATHER_TEMPERATURE_C);
  Tuple *temperatureF = dict_find(received, WEATHER_TEMPERATURE_F);
  Tuple *icon = dict_find(received, WEATHER_ICON);
  Tuple *stock = dict_find(received, LOCATION);
  Tuple *stock_change = dict_find(received, WEATHER_ERROR);
  
  str_temp = temperature->value->cstring;
  str_temp_F = temperatureF->value->cstring;
  str_stock = stock->value->cstring;
  
  //strncpy(str_stock_change, stock_change->value->cstring, 6);
   
  disp_update();
 
  //if (temperature) {
    
    
  //}

  if (icon) {
    // figure out which resource to use
    int8_t id = icon->value->int8;
    if (image != NULL) {
      gbitmap_destroy(image);
      layer_remove_from_parent(bitmap_layer_get_layer(image_layer));
      bitmap_layer_destroy(image_layer);
    }

    Layer *window_layer = window_get_root_layer(window);

    image = gbitmap_create_with_resource(IMAGE_RESOURCE_IDS[id]);
    image_layer = bitmap_layer_create(GRect(2, 168-26, 24, 24));
    bitmap_layer_set_bitmap(image_layer, image);
    layer_add_child(window_layer, bitmap_layer_get_layer(image_layer));
    
  }
}

//Start Battery code

/*
 * Status icon callback handler
 */
static void status_layer_update_callback(Layer *layer, GContext *ctx) {

  graphics_context_set_compositing_mode(ctx, GCompOpAssign);

  if (g_status_display == STATUS_REQUEST) {
    graphics_draw_bitmap_in_rect(ctx, icon_status_1, GRect(0, 0, 15, 15));
  } else if (g_status_display == STATUS_REPLY) {
    graphics_draw_bitmap_in_rect(ctx, icon_status_2, GRect(0, 0, 15, 15));
  }
}

/*
 * Set the status
 */
void set_status(int new_status_display) {
  g_status_display = new_status_display;
  layer_mark_dirty(status_layer);
}

/*
 * Pebble Battery icon callback handler
 */
static void p_battery_layer_update_callback(Layer *layer, GContext *ctx) {

  graphics_context_set_compositing_mode(ctx, GCompOpAssign);
   snprintf(battery_level_string, 5, "%d", battery_level);
  text_layer_set_text(battery_level_layer, battery_level_string);
  //text_layer_set_text(text_unit_layer, unit_layer);
  
  
  if (!battery_plugged) {
    graphics_draw_bitmap_in_rect(ctx, icon_battery_normal, GRect(0, 0, 35, 15));
    graphics_context_set_stroke_color(ctx, GColorBlack);
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_fill_rect(ctx, GRect(16, 5, (uint8_t)((battery_level / 100.0) * 11.0), 4), 0, GCornerNone);
    } else {
    graphics_draw_bitmap_in_rect(ctx, icon_battery_charge, GRect(0, 0, 35, 15));
  }
}

/*
 * Battery state change
 */
static void battery_state_handler(BatteryChargeState charge) {
  battery_level = charge.charge_percent;
  battery_plugged = charge.is_plugged;
  layer_mark_dirty(p_battery_layer);
  //vibes_short_pulse();
}
// End Battery code


// bluettoth code

  static void Bluetooth_Connection_Handler(bool connected) {
    //Bluetooth_on_off = bluetooth_connection_service_peek();
    if (connected) {
 //Create bluetooth pic 
  bitmap_layer_set_bitmap(image_layer_BT, image_BT);
  }  else {
  bitmap_layer_set_bitmap(image_layer_BT, image_error);
   
   
      }
    vibes_short_pulse();
    
  }

/*void force_update(void) {
    Bluetooth_Connection_Handler(bluetooth_connection_service_peek());
    }
*/

static void window_load(Window *window) {
  

  Layer *window_layer = window_get_root_layer(window);
    
  font49 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_49));
  font39 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_39));
  font21B = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_24));
  font21 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21));
  font19 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_19));
  font16 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_16));
  font12 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_12));
  font10 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_10));
  
  icon_battery_normal = gbitmap_create_with_resource(RESOURCE_ID_WATCH_BATTERY_NORMAL);
  icon_battery_charge = gbitmap_create_with_resource(RESOURCE_ID_WATCH_BATTERY_CHARGE);
  image_error = gbitmap_create_with_resource(RESOURCE_ID_ERROR);
  

  //Create background pic 
  image_BMW = gbitmap_create_with_resource(RESOURCE_ID_brazil_flag);
  image_layer_BMW = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(image_layer_BMW, image_BMW);
  layer_add_child(window_layer, bitmap_layer_get_layer(image_layer_BMW));
    
   // Pebble Battery
  BatteryChargeState initial = battery_state_service_peek();
  battery_level = initial.charge_percent;
  battery_plugged = initial.is_plugged;
  p_battery_layer = layer_create(GRect(112,0,35,15));
  layer_set_update_proc(p_battery_layer, p_battery_layer_update_callback);
  layer_add_child(window_get_root_layer(window), p_battery_layer);
  
 //Create bluetooth pic 
  image_BT = gbitmap_create_with_resource(RESOURCE_ID_BLUETOOTH_ON);
  image_layer_BT = bitmap_layer_create(GRect(0, 0, 25, 25));
  bitmap_layer_set_bitmap(image_layer_BT, image_BT);
  layer_add_child(window_layer, bitmap_layer_get_layer(image_layer_BT));
     
    
  
  // create battery level layer - this is where time goes
  battery_level_layer = text_layer_create(GRect(100, 0, 20, 15));
  text_layer_set_text_alignment(battery_level_layer, GTextAlignmentRight);
  text_layer_set_text_color(battery_level_layer, GColorWhite);
  text_layer_set_background_color(battery_level_layer, GColorClear);
  text_layer_set_font(battery_level_layer, font12);
  layer_add_child(window_layer, text_layer_get_layer(battery_level_layer));

  
  // create hour layer - this is where time goes
  text_time_layer = text_layer_create(GRect(0, 13, 144, 70));
  text_layer_set_text_alignment(text_time_layer, GTextAlignmentCenter);
  text_layer_set_text_color(text_time_layer, GColorWhite);
  text_layer_set_background_color(text_time_layer, GColorClear);
  text_layer_set_font(text_time_layer, font39);
  layer_add_child(window_layer, text_layer_get_layer(text_time_layer));

  
  
  // create minute layer - this is where time goes
  text_minute_layer = text_layer_create(GRect(75, 13, 70, 70));
  text_layer_set_text_color(text_minute_layer, GColorWhite);
  text_layer_set_background_color(text_minute_layer, GColorClear);
  text_layer_set_font(text_minute_layer, font39);
  layer_add_child(window_layer, text_layer_get_layer(text_minute_layer));
  
  // create wkday layer - this is where time goes
  text_wkday_layer = text_layer_create(GRect(0, 110, 144, 50));
  text_layer_set_text_alignment(text_wkday_layer, GTextAlignmentCenter);
  text_layer_set_text_color(text_wkday_layer, GColorWhite);
  text_layer_set_background_color(text_wkday_layer, GColorClear);
  text_layer_set_font(text_wkday_layer, font19);
  layer_add_child(window_layer, text_layer_get_layer(text_wkday_layer));
  
  // create day layer - this is where time goes
  text_day_layer = text_layer_create(GRect(90, 110, 28, 28));
  text_layer_set_text_alignment(text_day_layer, GTextAlignmentRight);
  text_layer_set_text_color(text_day_layer, GColorWhite);
  text_layer_set_background_color(text_day_layer, GColorClear);
  text_layer_set_font(text_day_layer, font19);
  layer_add_child(window_layer, text_layer_get_layer(text_day_layer));
  
  
  // create month layer - this is where the date goes
  text_date_layer = text_layer_create(GRect(52, 110, 50, 30));
  text_layer_set_text_alignment(text_date_layer, GTextAlignmentCenter);
  text_layer_set_text_color(text_date_layer, GColorWhite);
  text_layer_set_background_color(text_date_layer, GColorClear);
  text_layer_set_font(text_date_layer, font19);
  layer_add_child(window_layer, text_layer_get_layer(text_date_layer));

  // create temperature layer - this is where the temperature goes
  text_temp_layer = text_layer_create(GRect(144-44, 168-26, 40, 40));
  text_layer_set_text_alignment(text_temp_layer, GTextAlignmentRight);
  text_layer_set_text_color(text_temp_layer, GColorWhite);
  text_layer_set_background_color(text_temp_layer, GColorClear);
  text_layer_set_font(text_temp_layer, font21);
  layer_add_child(window_layer, text_layer_get_layer(text_temp_layer));
  
  // create temperature label layer - this is where the temperature goes
  text_unit_layer = text_layer_create(GRect(140-15, 130, 15, 15));
  text_layer_set_text_alignment(text_unit_layer, GTextAlignmentRight);
  text_layer_set_text_color(text_unit_layer, GColorWhite);
  text_layer_set_background_color(text_unit_layer, GColorClear);
  text_layer_set_font(text_unit_layer, font12);
  layer_add_child(window_layer, text_layer_get_layer(text_unit_layer));

    // create stock label layer - this is where the temperature goes
  text_stock_layer = text_layer_create(GRect(47,142, 50, 22));
  text_layer_set_text_alignment(text_stock_layer, GTextAlignmentCenter);
  text_layer_set_text_color(text_stock_layer, GColorWhite);
  text_layer_set_background_color(text_stock_layer, GColorClear);
  text_layer_set_font(text_stock_layer, font16);
  layer_add_child(window_layer, text_layer_get_layer(text_stock_layer));
  
}

static void window_unload(Window *window) {
  // destroy the text layers - this is good
  text_layer_destroy(text_date_layer);
  text_layer_destroy(text_time_layer);
  text_layer_destroy(text_temp_layer);
  text_layer_destroy(text_minute_layer);
  text_layer_destroy(text_day_layer);
  text_layer_destroy(text_wkday_layer);
  text_layer_destroy(battery_level_layer);
  text_layer_destroy(text_unit_layer);
  text_layer_destroy(text_stock_layer);
  
  layer_destroy(p_battery_layer);
  gbitmap_destroy(icon_battery_normal);
  gbitmap_destroy(icon_battery_charge);
  
  // destroy the image layers
  gbitmap_destroy(image);
  layer_remove_from_parent(bitmap_layer_get_layer(image_layer));
  bitmap_layer_destroy(image_layer);

  gbitmap_destroy(image_BMW);
  layer_remove_from_parent(bitmap_layer_get_layer(image_layer_BMW));
  bitmap_layer_destroy(image_layer_BMW);
  
  gbitmap_destroy(image_BT);
  layer_remove_from_parent(bitmap_layer_get_layer(image_layer_BT));
  bitmap_layer_destroy(image_layer_BT);
  
  // unload the fonts
  fonts_unload_custom_font(font49);
  fonts_unload_custom_font(font39);
  fonts_unload_custom_font(font21);
  fonts_unload_custom_font(font21B);
  fonts_unload_custom_font(font19);
  fonts_unload_custom_font(font10);
  fonts_unload_custom_font(font12);
  fonts_unload_custom_font(font16);
}


static void app_message_init(void) {
  app_message_open(64, 64);
  app_message_register_inbox_received(in_received_handler);
  
  
}



void handle_tap(AccelAxisType axis, int32_t direction) {
    
  //vibes_short_pulse();
  disp_update();
}

// show the date and time every minute
void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  // Need to be static because they're used by the system later.
  static char hour_text[] = "x0x00";
  static char minute_text[] = "00";
  static char day_text[] = "00";
  static char month_text[] = "Xxx";
  static char wkday_text[] = "XxxxXxxxxx";

 
  char *time_format;

  strftime(hour_text, sizeof(hour_text), "%R", tick_time);
  text_layer_set_text(text_time_layer, hour_text);

  //strftime(minute_text, sizeof(minute_text), "%M", tick_time);
  //text_layer_set_text(text_minute_layer, minute_text);
  
  //strftime(month_text, sizeof(month_text), "%b", tick_time);
  //text_layer_set_text(text_date_layer, month_text);



  //strftime(day_text, sizeof(day_text), "%e", tick_time);
  //text_layer_set_text(text_day_layer, day_text);

  //text_layer_set_text(text_day_layer, "30");
 
  
  strftime(wkday_text, sizeof(wkday_text), "%c", tick_time);
  text_layer_set_text(text_wkday_layer, wkday_text);
  
  //text_layer_set_text(text_stock_layer, str_stock);
  
}   

//void handle_hour_tick(struct tm *tick_time, TimeUnits units_changed) {
  //force_update();
  //app_message_init();
 
//}   


static void init(void) {
  window = window_create();
  app_message_init();

  window_set_background_color(window, GColorBlack);

  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);

  // subscribe to update every minute
  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
  //tick_timer_service_subscribe(HOUR_UNIT, handle_hour_tick);  
  
  battery_state_service_subscribe(battery_state_handler);
  bluetooth_connection_service_subscribe(Bluetooth_Connection_Handler);
  accel_tap_service_subscribe(handle_tap);

}

static void deinit(void) {
  tick_timer_service_unsubscribe();
  app_message_deregister_callbacks();

  window_stack_remove(window, true);

  window_destroy(window);
   battery_state_service_unsubscribe();
   bluetooth_connection_service_unsubscribe();
  accel_tap_service_unsubscribe();

  
}

int main(void) {
  init();

  app_event_loop();
  deinit();
}

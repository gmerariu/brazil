
#include <pebble.h>
#include "common.h"
#include "pebble_fonts.h"


static Window *window;
TextLayer *text_date_layer;
TextLayer *text_time_layer;
TextLayer *text_temp_layer; 

TextLayer *text_unit_layer;
static char *unit_layer="C";

static char *str_temp;
static char *str_temp_F;
static char *str_temp_min;
static char *str_temp_max;
static char *str_feels_like;
static char *str_wind_speed;
static char *str_stock = " ";
static char *str_stock_change = " ";

static char *str_wind = "W";
static char *str_feels = "F";
static char *str_min = "<";
static char *str_max = ">";


TextLayer *text_day_layer;
TextLayer *text_minute_layer;
TextLayer *text_wkday_layer;
TextLayer *battery_level_layer;

TextLayer *text_temp_min;
TextLayer *text_temp_max;
TextLayer *text_feels_like;
TextLayer *text_wind_speed;

TextLayer *min;
TextLayer *max;
TextLayer *wind;
TextLayer *feels;

TextLayer *text_stock_layer;
TextLayer *text_stock_change_layer;

//GFont *font49;
GFont *font39;
GFont *font21;
//GFont *font21B;
GFont *font19;
//GFont *font16;
GFont *font10;
GFont *font12;

static GBitmap *image = NULL;
GBitmap *image_BMW = NULL;
GBitmap *image_BR = NULL;
static GBitmap *image_BT = NULL;
static GBitmap *image_seconds = NULL;
static GBitmap *image_error = NULL;
static GBitmap *icon_battery_normal;
static GBitmap *icon_battery_charge; 
void set_battery(uint8_t state, int8_t level);


static Layer *p_battery_layer;
static Layer *seconds_layer;
static int g_status_display = 0;
static Layer *status_layer;

static BitmapLayer *image_layer;
static BitmapLayer *image_layer_BMW;
static BitmapLayer *image_layer_BR;
static BitmapLayer *image_layer_BT;
static BitmapLayer *image_seconds_layer;

static BitmapLayer *reset_update_layer;

static uint8_t battery_level;
static bool battery_plugged;
static char battery_level_string[5];

static int second_text =0;
static int reset_update_timer=1;

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
  TEMP_MIN,
  TEMP_MAX,
  FEELS_LIKE,
  WIND_SPEED
};

void disp_update(void){
       
 //char str_time_updated2[]="00"; 
  
  if (str_temp) {
     unit_layer="C";
  } else {
      unit_layer=" ";
    }
        
      text_layer_set_text(text_unit_layer, unit_layer);
      text_layer_set_text(text_temp_layer, str_temp);    
      text_layer_set_text(text_stock_layer, str_stock);
      text_layer_set_text(text_stock_change_layer, str_stock_change);
      text_layer_set_text(text_temp_min, str_temp_min); 
      text_layer_set_text(text_temp_max, str_temp_max);
      text_layer_set_text(text_feels_like, str_feels_like);   
      text_layer_set_text(text_wind_speed, str_wind_speed); 
  
      text_layer_set_text(wind, str_wind); 
      text_layer_set_text(feels, str_feels); 
      text_layer_set_text(min, str_min); 
      text_layer_set_text(max, str_max); 
  


}

void in_received_handler(DictionaryIterator *received, void *context) {
  // incoming message received
  
  
  reset_update_timer=1;

  
  // Celsius or Fahrenheit?
  
  Tuple *temperature = dict_find(received, WEATHER_TEMPERATURE_C);
  Tuple *temperatureF = dict_find(received, WEATHER_TEMPERATURE_F);
  Tuple *icon = dict_find(received, WEATHER_ICON);
  Tuple *stock = dict_find(received, LOCATION);
  Tuple *stock_change = dict_find(received, WEATHER_ERROR);
  Tuple *temp_min = dict_find(received, TEMP_MIN);
  Tuple *temp_max = dict_find(received, TEMP_MAX);
  Tuple *feels_like = dict_find(received, FEELS_LIKE);
  Tuple *wind_speed = dict_find(received, WIND_SPEED);
  
  
  str_temp = temperature->value->cstring;
  str_temp_F = temperatureF->value->cstring;
  str_stock = stock->value->cstring;
  str_stock_change = stock_change->value->cstring;
  str_temp_min = temp_min->value->cstring;
  str_temp_max = temp_max->value->cstring;
  str_feels_like = feels_like->value->cstring;
  str_wind_speed = wind_speed->value->cstring;
  
 
  //strncpy(str_stock_change, stock_change->value->cstring, 6);
   
    
    disp_update();

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

static void status_layer_update_callback(Layer *layer, GContext *ctx) {

  graphics_context_set_compositing_mode(ctx, GCompOpAssign);

  if (g_status_display == STATUS_REQUEST) {
    graphics_draw_bitmap_in_rect(ctx, icon_status_1, GRect(0, 0, 15, 15));
  } else if (g_status_display == STATUS_REPLY) {
    graphics_draw_bitmap_in_rect(ctx, icon_status_2, GRect(0, 0, 15, 15));
  }
}
 */

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
    graphics_fill_rect(ctx, GRect(16, 4, (uint8_t)((battery_level / 100.0) * 11.0), 6), 0, GCornerNone);
    } else {
    graphics_draw_bitmap_in_rect(ctx, icon_battery_charge, GRect(0, 0, 35, 15));
  }
}



static void seconds_layer_update_callback(Layer *layer, GContext *ctx) {
    
  // fill update time bar  
    graphics_draw_bitmap_in_rect(ctx, image_seconds, GRect(34,8,64,8));
    graphics_context_set_compositing_mode(ctx, GCompOpAssign);
    graphics_context_set_stroke_color(ctx, GColorClear);
    
    
  if (reset_update_timer<=1800){
      graphics_context_set_fill_color(ctx, GColorWhite);  
      graphics_fill_rect(ctx, GRect(36, 10, reset_update_timer/30,4), 0, GCornersAll); 
  } else {

      graphics_context_set_fill_color(ctx, GColorBlack);
      graphics_fill_rect(ctx, GRect(36, 10, 60,4), 0, GCornersAll); 
      graphics_context_set_fill_color(ctx, GColorWhite);
      graphics_fill_rect(ctx, GRect(36, 11, (reset_update_timer-1800)/6,1), 0, GCornersAll); 
  }
    
  
  // fill seconds bar
    graphics_draw_bitmap_in_rect(ctx, image_seconds, GRect(34, 0, 64, 8));
    graphics_context_set_compositing_mode(ctx, GCompOpAssign);
    graphics_context_set_stroke_color(ctx, GColorClear);
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_fill_rect(ctx, GRect(36, 1, second_text, 5), 0, GCornersAll);
  
  
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
    
  //font49 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_49));
  font39 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_39));
  //font21B = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_BOLD_SUBSET_24));
  font21 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_21));
  font19 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_19));
  //font16 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_16));
  font12 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_12));
  font10 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTO_CONDENSED_10));
  
  
  icon_battery_normal = gbitmap_create_with_resource(RESOURCE_ID_WATCH_BATTERY_NORMAL);
  icon_battery_charge = gbitmap_create_with_resource(RESOURCE_ID_WATCH_BATTERY_CHARGE);
  image_error = gbitmap_create_with_resource(RESOURCE_ID_ERROR);
  image_seconds = gbitmap_create_with_resource(RESOURCE_ID_SECONDS_IMAGE);
  

  //Create background pic 
  image_BMW = gbitmap_create_with_resource(RESOURCE_ID_brazil_flag);
  image_layer_BMW = bitmap_layer_create(GRect(0, 0, 144, 168));
  bitmap_layer_set_bitmap(image_layer_BMW, image_BMW);
  layer_add_child(window_layer, bitmap_layer_get_layer(image_layer_BMW));
  
    //Create background seconds box
  image_seconds = gbitmap_create_with_resource(RESOURCE_ID_SECONDS_IMAGE);
  image_seconds_layer = bitmap_layer_create(GRect(34, 3, 64, 8));
  bitmap_layer_set_bitmap(image_seconds_layer, image_seconds);
  layer_add_child(window_layer, bitmap_layer_get_layer(image_seconds_layer));
    
   // Pebble Battery
  BatteryChargeState initial = battery_state_service_peek();
  battery_level = initial.charge_percent;
  battery_plugged = initial.is_plugged;
  p_battery_layer = layer_create(GRect(111,0,35,15));
  layer_set_update_proc(p_battery_layer, p_battery_layer_update_callback);
  layer_add_child(window_get_root_layer(window), p_battery_layer);
  
 
  // Seconds Box
  seconds_layer = layer_create(GRect(0,0,144,15));
  layer_set_update_proc(seconds_layer, seconds_layer_update_callback);
  layer_add_child(window_get_root_layer(window), seconds_layer);
 
  
 //Create bluetooth pic 
  image_BT = gbitmap_create_with_resource(RESOURCE_ID_BLUETOOTH_ON);
  image_layer_BT = bitmap_layer_create(GRect(0, 0, 25, 15));
  bitmap_layer_set_bitmap(image_layer_BT, image_BT);
  layer_add_child(window_layer, bitmap_layer_get_layer(image_layer_BT));
     
    
  
  // create battery level layer 
  battery_level_layer = text_layer_create(GRect(103, 0, 20, 15));
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
  text_layer_set_font(text_wkday_layer, font21);
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
  text_temp_layer = text_layer_create(GRect(140-44, 168-30, 40, 40));
  text_layer_set_text_alignment(text_temp_layer, GTextAlignmentRight);
  text_layer_set_text_color(text_temp_layer, GColorWhite);
  text_layer_set_background_color(text_temp_layer, GColorClear);
  text_layer_set_font(text_temp_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(text_temp_layer));
  
  // create temperature label layer - this is where the temperature goes
  text_unit_layer = text_layer_create(GRect(140-15, 135, 15, 15));
  text_layer_set_text_alignment(text_unit_layer, GTextAlignmentRight);
  text_layer_set_text_color(text_unit_layer, GColorWhite);
  text_layer_set_background_color(text_unit_layer, GColorClear);
  text_layer_set_font(text_unit_layer, font10);
  layer_add_child(window_layer, text_layer_get_layer(text_unit_layer));

    // create stock label layer - this is where the temperature goes
  text_stock_layer = text_layer_create(GRect(0,132, 144, 25));
  text_layer_set_text_alignment(text_stock_layer, GTextAlignmentCenter);
  text_layer_set_text_color(text_stock_layer, GColorWhite);
  text_layer_set_background_color(text_stock_layer, GColorClear);
  text_layer_set_font(text_stock_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
  layer_add_child(window_layer, text_layer_get_layer(text_stock_layer));
  
      // create stock CHANGE label layer - this is where the temperature goes
  text_stock_change_layer = text_layer_create(GRect(0,148, 144, 18));
  text_layer_set_text_alignment(text_stock_change_layer, GTextAlignmentCenter);
  text_layer_set_text_color(text_stock_change_layer, GColorWhite);
  text_layer_set_background_color(text_stock_change_layer, GColorClear);
  text_layer_set_font(text_stock_change_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  layer_add_child(window_layer, text_layer_get_layer(text_stock_change_layer));
  //fonts_get_system_font(FONT_KEY_GOTHIC_14)
  
    // create temperature MIN layer 
  text_temp_min = text_layer_create(GRect(35, 58, 25, 20));
  text_layer_set_text_alignment(text_temp_min, GTextAlignmentLeft);
  text_layer_set_text_color(text_temp_min, GColorWhite);
  text_layer_set_background_color(text_temp_min, GColorClear);
  text_layer_set_font(text_temp_min, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  layer_add_child(window_layer, text_layer_get_layer(text_temp_min));
  
      // create temperature MAX layer 
  text_temp_max = text_layer_create(GRect(85, 58, 25, 20));
  text_layer_set_text_alignment(text_temp_max, GTextAlignmentRight);
  text_layer_set_text_color(text_temp_max, GColorWhite);
  text_layer_set_background_color(text_temp_max, GColorClear);
  text_layer_set_font(text_temp_max, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  layer_add_child(window_layer, text_layer_get_layer(text_temp_max));
  
        // create temperature feels_like layer 
  text_feels_like = text_layer_create(GRect(85, 85, 25, 20));
  text_layer_set_text_alignment(text_feels_like, GTextAlignmentRight);
  text_layer_set_text_color(text_feels_like, GColorWhite);
  text_layer_set_background_color(text_feels_like, GColorClear);
  text_layer_set_font(text_feels_like, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  layer_add_child(window_layer, text_layer_get_layer(text_feels_like));
  
          // create temperature wind_speed layer 
  text_wind_speed = text_layer_create(GRect(35, 85, 25, 20));
  text_layer_set_text_alignment(text_wind_speed, GTextAlignmentLeft);
  text_layer_set_text_color(text_wind_speed, GColorWhite);
  text_layer_set_background_color(text_wind_speed, GColorClear);
  text_layer_set_font(text_wind_speed, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  layer_add_child(window_layer, text_layer_get_layer(text_wind_speed));
  
            // create temperature wind layer 
  wind = text_layer_create(GRect(16, 85, 25, 20));
  text_layer_set_text_alignment(wind, GTextAlignmentLeft);
  text_layer_set_text_color(wind, GColorWhite);
  text_layer_set_background_color(wind, GColorClear);
  text_layer_set_font(wind, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  layer_add_child(window_layer, text_layer_get_layer(wind));
  
              // create temperature feels layer 
  feels = text_layer_create(GRect(100, 85, 25, 20));
  text_layer_set_text_alignment(feels, GTextAlignmentRight);
  text_layer_set_text_color(feels, GColorWhite);
  text_layer_set_background_color(feels, GColorClear);
  text_layer_set_font(feels, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  layer_add_child(window_layer, text_layer_get_layer(feels));
  
        // create temperature MAX layer 
  max = text_layer_create(GRect(100, 58, 25, 20));
  text_layer_set_text_alignment(max, GTextAlignmentRight);
  text_layer_set_text_color(max, GColorWhite);
  text_layer_set_background_color(max, GColorClear);
  text_layer_set_font(max, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  layer_add_child(window_layer, text_layer_get_layer(max));
  
          // create temperature Min layer 
  min = text_layer_create(GRect(18, 58, 25, 20));
  text_layer_set_text_alignment(min, GTextAlignmentLeft);
  text_layer_set_text_color(min, GColorWhite);
  text_layer_set_background_color(min, GColorClear);
  text_layer_set_font(min, fonts_get_system_font(FONT_KEY_GOTHIC_18));
  layer_add_child(window_layer, text_layer_get_layer(min));
  
  
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
  text_layer_destroy(text_stock_change_layer);
  text_layer_destroy(text_temp_min);
  text_layer_destroy(text_temp_max);
  text_layer_destroy(text_feels_like);
  text_layer_destroy(text_wind_speed);
    text_layer_destroy(min);
    text_layer_destroy(max);
    text_layer_destroy(wind);
    text_layer_destroy(feels);
  
  layer_destroy(p_battery_layer);
  
  
    // destroy the image layers
  gbitmap_destroy(icon_battery_normal);
  gbitmap_destroy(icon_battery_charge);
  
  //gbitmap_destroy(image_reset_update);
  layer_remove_from_parent(bitmap_layer_get_layer(reset_update_layer));
  bitmap_layer_destroy(reset_update_layer);
       

  gbitmap_destroy(image);
  layer_remove_from_parent(bitmap_layer_get_layer(image_layer));
  bitmap_layer_destroy(image_layer);

  gbitmap_destroy(image_BMW);
  layer_remove_from_parent(bitmap_layer_get_layer(image_layer_BMW));
  bitmap_layer_destroy(image_layer_BMW);
  
  gbitmap_destroy(image_BR);
  layer_remove_from_parent(bitmap_layer_get_layer(image_layer_BR));
  bitmap_layer_destroy(image_layer_BR);
  
  gbitmap_destroy(image_seconds);
  layer_remove_from_parent(bitmap_layer_get_layer(image_seconds_layer));
  bitmap_layer_destroy(image_seconds_layer);
  
  
  gbitmap_destroy(image_BT);
  layer_remove_from_parent(bitmap_layer_get_layer(image_layer_BT));
  bitmap_layer_destroy(image_layer_BT);
  
  // unload the fonts
  //fonts_unload_custom_font(font49);
  fonts_unload_custom_font(font39);
  fonts_unload_custom_font(font21);
  //fonts_unload_custom_font(font21B);
  fonts_unload_custom_font(font19);
  fonts_unload_custom_font(font10);
  fonts_unload_custom_font(font12);
  //fonts_unload_custom_font(font16);
}


static void app_message_init(void) {
  
  app_message_register_inbox_received(in_received_handler);
  //app_message_open(128, 128);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
}



void handle_tap(AccelAxisType axis, int32_t direction) {
    
  //vibes_short_pulse();
  disp_update();
}

// show the date and time every minute
void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed) {
  // Need to be static because they're used by the system later.
  static char hour_text[] = "x0x00";
  //static char minute_text[] = "00";
  //static char day_text[] = "00";
  //static char month_text[] = "Xxx";
  static char wkday_text[] = "XxxxXxxxxx";

 
  //char *time_format;

  strftime(hour_text, sizeof(hour_text), "%r", tick_time);
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



void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {

  static char second_text_char[] = "00";
  
    strftime(second_text_char, sizeof(second_text_char), "%S", tick_time); 
    second_text = atoi(second_text_char);
  
  
  static char hour_text[] = "x0x00";
  static char wkday_text[] = "XxxxXxxxxx";

  strftime(hour_text, sizeof(hour_text), "%r", tick_time);
  text_layer_set_text(text_time_layer, hour_text);

  strftime(wkday_text, sizeof(wkday_text), "%c", tick_time);
  text_layer_set_text(text_wkday_layer, wkday_text);
  
  if (reset_update_timer <= 2160){
    reset_update_timer = reset_update_timer +1;
  }   
  
    disp_update();

}



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
  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
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

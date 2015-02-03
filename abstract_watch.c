

#include "pebble.h"

Window *window;
Layer *display_layer;

// Defines for the 4 x 3 table of circles representing hour
#define HOUR_CIRCLE_RADIUS_PIXELS         16
#define HOUR_CIRCLE_EDGE_STROKE_PIXELS 2

#define HOUR_CIRCLE_PADDING_PIXELS 18 - HOUR_CIRCLE_RADIUS_PIXELS
#define HOUR_CIRCLE_CELL_SIZE     (2 * (HOUR_CIRCLE_RADIUS_PIXELS + HOUR_CIRCLE_PADDING_PIXELS))

#define HOUR_CELLS_PER_ROW     4
#define HOUR_CELLS_PER_COLUMN  3

#define HOURS_ONE_FOUR_ROW    0
#define HOURS_FIVE_EIGHT_ROW  1
#define HOURS_NINE_TWELVE_ROW 2
#define HOURS_NUM_COLS        4
#define HOURS_NUM_ROWS        4

// Defines for the bar representing minute
#define MAX_MINUTE_BAR_VAL 144  
  
void draw_and_fill_circle(GContext* ctx, GPoint center, bool fillcircle) {
  
  graphics_context_set_fill_color(ctx, GColorWhite);

  graphics_fill_circle(ctx, center, HOUR_CIRCLE_RADIUS_PIXELS);

  if (!fillcircle) {
    graphics_context_set_fill_color(ctx, GColorBlack);
    graphics_fill_circle(ctx, center, HOUR_CIRCLE_RADIUS_PIXELS - HOUR_CIRCLE_EDGE_STROKE_PIXELS);
  }

}

void draw_minute_bar(Layer *me, GContext* ctx, unsigned short minute_value) {
    
    //draw rectangle to represent 60 minutes
    GRect minute_bar_rect;
    minute_bar_rect.origin = GPoint(0,130);
    minute_bar_rect.size = GSize(144,16);
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_draw_rect(ctx, rect1);
        
    //draw a filled rectangle to represent the minute
    unsigned short minute_fill = (minute_value * MAX_MINUTE_BAR_VAL) / 60;
    
    GRect minute_fill_rect;
    minute_fill_rect.origin = GPoint(0,130);
    minute_fill_rect.size = GSize(minute_fill,16);
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_fill_rect(ctx, rect3, 0, GCornerNone);
}

GPoint get_circle_center(unsigned short x, unsigned short y) {
  return GPoint((HOUR_CIRCLE_CELL_SIZE/2) + (HOUR_CIRCLE_CELL_SIZE * x),
		(HOUR_CIRCLE_CELL_SIZE/2) + (HOUR_CIRCLE_CELL_SIZE * y));
}


void draw_table_for_hour(GContext* ctx, unsigned short hour_value, unsigned short max_columns, unsigned short row) {
  for (int cell_column_index = 0; cell_column_index< max_columns; cell_column_index++) {
    draw_and_fill_circle(ctx,get_circle_center(cell_column_index, row), ((hour_value - (HOURS_NUM_ROWS * row) - cell_column_index) > 0));
  }
}


unsigned short get_display_hour(unsigned short hour) {
  unsigned short display_hour = hour % 12;

  // Convert "0" hour to "12"
  return display_hour ? display_hour : 12;

}


void display_layer_update_callback(Layer *me, GContext* ctx) {

  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  unsigned short display_hour = get_display_hour(t->tm_hour);
  

  draw_table_for_hour(ctx, display_hour, HOURS_NUM_COLS, HOURS_ONE_FOUR_ROW);
  draw_table_for_hour(ctx, display_hour, HOURS_NUM_COLS, HOURS_FIVE_EIGHT_ROW);
  draw_table_for_hour(ctx, display_hour, HOURS_NUM_COLS, HOURS_NINE_TWELVE_ROW);
  
  draw_minute_bar(me, ctx, t->tm_min);


static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(display_layer);
}

static void do_init(void) {
  window = window_create();
  window_stack_push(window, true);

  window_set_background_color(window, GColorBlack);

  Layer *root_layer = window_get_root_layer(window);
  GRect frame = layer_get_frame(root_layer);

  // Init the layer for the display
  display_layer = layer_create(frame);
  layer_set_update_proc(display_layer, &display_layer_update_callback);
  layer_add_child(root_layer, display_layer);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void do_deinit(void) {
  layer_destroy(display_layer);
  window_destroy(window);
}


int main(void) {
  do_init();
  app_event_loop();
  do_deinit();
}



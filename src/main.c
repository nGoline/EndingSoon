#include <pebble.h>
  
static Window *s_main_window;
static TextLayer *s_time_layer;
static TextLayer *s_day_layer;
static TextLayer *s_month_layer;
static Layer *s_canvas_layer;
static int day_pixels = 0;
static int month_pixels = 0;
static int year_pixels = 0;

static int days_in_month(int month, int year) {
	int numberOfDays;
	if (month == 4 || month == 6 || month == 9 || month == 11)
  		numberOfDays = 30;
	else if (month == 2) {
		bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
		if (isLeapYear)
    		numberOfDays = 29;
  		else
    		numberOfDays = 28;
	}
	else
        numberOfDays = 31;
	
	return numberOfDays;
}

static void calculate_day(struct tm * tick_time) {
	int minutes = 0;
  	char hour_b[] = "00";
	char time[] = "00:00";

	// Write the current hours and minutes into the buffer
	if(clock_is_24h_style() == true) {
		// Use 24 hour format
		strftime(time, sizeof time, "%H:%M", tick_time);
	} else {
		// Use 12 hour format
		strftime(time, sizeof time, "%I:%M", tick_time);
	}
	//get hour
    strftime(hour_b, sizeof hour_b, "%H", tick_time);
	minutes = atoi(hour_b);
	minutes = minutes * 60;
	//get minute
    strftime(hour_b, sizeof hour_b, "%M", tick_time);	
	minutes += atoi(hour_b);
	//get pixels of the day
	double day_multiplier = (100 - ((1440 - minutes) / 14.40));	
	day_pixels = (int)(day_multiplier * 1.24);
	
	text_layer_set_text(s_time_layer, time);
}

static void calculate_month_year(struct tm * tick_time) {
	int day = 0;
	int month = 0;
	int year = 0;
  	char day_b[] = "00";
	char month_b[] = "00";
	char year_b[] = "0000";

	//get day
    strftime(day_b, sizeof day_b, "%d", tick_time);
	text_layer_set_text(s_day_layer, day_b);
	day = atoi(day_b);
	//get month
    strftime(month_b, sizeof month_b, "%m", tick_time);
	text_layer_set_text(s_month_layer, month_b);
	month = atoi(month_b);
	//get year
    strftime(year_b, sizeof year_b, "%Y", tick_time);	
	year = atoi(year_b);
	//get pixels of the month
	int month_days = days_in_month(month, year);
	double month_multiplier = 100 - ((month_days - day) / (month_days * 0.01));	
	month_pixels = (int)(month_multiplier * 1.24);
	//get pixels of the year
	double year_multiplier = 100 - ((12 - month) / 0.12);	
	year_pixels = (int)(year_multiplier * 1.24);
}

static void update_time() {
  	// Get a tm structure
  	time_t temp = time(NULL); 
  	struct tm * tick_time = localtime(&temp);
	
	calculate_day(tick_time);
	calculate_month_year(tick_time);
	
	// Mark layer dirty to force a refresh
  	layer_mark_dirty(s_canvas_layer);
}

static void update_graphic_proc(Layer *this_layer, GContext *ctx) {
	GPath *s_my_path_ptr = NULL;
	GPathInfo BOLT_PATH_INFO = {
		.num_points = 4,
	  	.points = (GPoint []) {{10,10},{134,10},{134,50},{10,50}}
	};
	s_my_path_ptr = gpath_create(&BOLT_PATH_INFO);
	graphics_context_set_stroke_color(ctx, GColorBlack);	
	gpath_draw_outline(ctx, s_my_path_ptr);
	
	GPathInfo BOLT_PATH_INFO2 = {
		.num_points = 4,
	  	.points = (GPoint []) {{10,65},{134,65},{134,105},{10,105}}
	};
	s_my_path_ptr = gpath_create(&BOLT_PATH_INFO2);
	graphics_context_set_stroke_color(ctx, GColorBlack);	
	gpath_draw_outline(ctx, s_my_path_ptr);
	
	GPathInfo BOLT_PATH_INFO3 = {
		.num_points = 4,
	  	.points = (GPoint []) {{10,120},{134,120},{134,160},{10,160}}
	};
	s_my_path_ptr = gpath_create(&BOLT_PATH_INFO3);
	
	graphics_context_set_stroke_color(ctx, GColorBlack);	
	gpath_draw_outline(ctx, s_my_path_ptr);
	GPathInfo BOLT_PATH_INFO4 = {
		.num_points = 4,
	  	.points = (GPoint []) {{10,10},{134,10},{134,50},{10,50}}
	};
	s_my_path_ptr = gpath_create(&BOLT_PATH_INFO4);
	graphics_context_set_stroke_color(ctx, GColorBlack);
	
	//day rectangle
 	graphics_fill_rect(ctx, GRect(10, 10, day_pixels, 40), 0, GCornerNone);
	//month rectangle
 	graphics_fill_rect(ctx, GRect(10, 65, month_pixels, 40), 0, GCornerNone);
	//year rectangle
 	graphics_fill_rect(ctx, GRect(10, 120, year_pixels, 40), 0, GCornerNone);
}

static void main_window_load(Window *window) {
  	// Create time TextLayer
	s_time_layer = text_layer_create(GRect(0, 10, 144, 50));
	text_layer_set_background_color(s_time_layer, GColorClear);
	text_layer_set_text_color(s_time_layer, GColorBlack);
	text_layer_set_text(s_time_layer, "YEAR");
  	// Create day TextLayer
	s_day_layer = text_layer_create(GRect(0, 65, 144, 50));
	text_layer_set_background_color(s_day_layer, GColorClear);
	text_layer_set_text_color(s_day_layer, GColorBlack);
	text_layer_set_text(s_day_layer, "ENDS");
  	// Create month TextLayer
	s_month_layer = text_layer_create(GRect(0, 112, 144, 50));
	text_layer_set_background_color(s_month_layer, GColorClear);
	text_layer_set_text_color(s_month_layer, GColorBlack);
	text_layer_set_text(s_month_layer, "SOON");

	// Improve the layout to be more like a watchface
	text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
	// Improve the layout to be more like a watchface
	text_layer_set_font(s_day_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	text_layer_set_text_alignment(s_day_layer, GTextAlignmentCenter);
	// Improve the layout to be more like a watchface
	text_layer_set_font(s_month_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
	text_layer_set_text_alignment(s_month_layer, GTextAlignmentCenter);
	
	Layer *s_hour_layer = window_get_root_layer(window);
  	GRect window_bounds = layer_get_bounds(s_hour_layer);

  	// Create Layer
  	s_canvas_layer = layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
  	layer_add_child(s_hour_layer, s_canvas_layer);
	// Add it as a child layer to the Window's root layer
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));
	// Add it as a child layer to the Window's root layer
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_day_layer));
	// Add it as a child layer to the Window's root layer
	layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_month_layer));

	// Set the update_proc
  	layer_set_update_proc(s_canvas_layer, update_graphic_proc);
	
	// Make sure the time is displayed from the start
	update_time();
}

static void main_window_unload(Window *window) {
  	// Destroy TextLayer
  	text_layer_destroy(s_time_layer);
	// Destroy Layer
  	layer_destroy(s_canvas_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	update_time();
}
  
static void init() {
	// Create main Window element and assign to pointer
	s_main_window = window_create();

	// Set handlers to manage the elements inside the Window
	window_set_window_handlers(s_main_window, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload
	});

	// Show the Window on the watch, with animated=true
	window_stack_push(s_main_window, true);
	
	// Register with TickTimerService
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
	// Destroy Window
	window_destroy(s_main_window);
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}
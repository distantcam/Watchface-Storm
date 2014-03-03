#include <pebble.h>
#include "autoconfig.h"

static Window *window;

static Layer *month_display_layer;
static Layer *day_display_layer;
static Layer *hour_display_layer;
static Layer *minute_display_layer;

static TextLayer *date_text_layer;
static TextLayer *time_text_layer;

void graphics_draw_arc(GContext *ctx, GPoint p, int radius, int thickness, int start, int end) {
	start = start % 360;
	end = end % 360;

	while (start < 0) start += 360;
	while (end < 0) end += 360;

	if (end == 0) end = 360;

	float sslope = (float)cos_lookup(start * TRIG_MAX_ANGLE / 360) / (float)sin_lookup(start * TRIG_MAX_ANGLE / 360);
	float eslope = (float)cos_lookup(end * TRIG_MAX_ANGLE / 360) / (float)sin_lookup(end * TRIG_MAX_ANGLE / 360);

	if (end == 360) eslope = -1000000;

	int ir2 = (radius - thickness) * (radius - thickness);
	int or2 = radius * radius;

	for (int x = -radius; x <= radius; x++)
		for (int y = -radius; y <= radius; y++)
		{
			int x2 = x * x;
			int y2 = y * y;

			if (
				(x2 + y2 < or2 && x2 + y2 >= ir2) &&
				(
					(y > 0 && start < 180 && x <= y * sslope) ||
					(y < 0 && start > 180 && x >= y * sslope) ||
					(y < 0 && start <= 180) ||
					(y == 0 && start <= 180 && x < 0) ||
					(y == 0 && start == 0 && x > 0)
				) &&
				(
					(y > 0 && end < 180 && x >= y * eslope) ||
					(y < 0 && end > 180 && x <= y * eslope) ||
					(y > 0 && end >= 180) ||
					(y == 0 && end >= 180 && x < 0) ||
					(y == 0 && start == 0 && x > 0)
				)
			)
				graphics_draw_pixel(ctx, GPoint(p.x + x, p.y + y));
		}
}

void month_display_layer_update_callback(Layer *me, GContext* ctx) {
	time_t now = time(NULL);
	struct tm *t = localtime(&now);

	unsigned int angle = (t->tm_mon + 1) * 15;

	GRect bounds = layer_get_bounds(me);
	GPoint center = grect_center_point(&bounds);
	center.y += 10;

	graphics_context_set_stroke_color(ctx, GColorWhite);

	graphics_draw_arc(ctx, center, 42, 20, 0, angle);

	if (getEndlines()) {
		graphics_draw_line(ctx, GPoint(7, center.y), GPoint(20, center.y));
		graphics_draw_line(ctx, GPoint(124, center.y), GPoint(137, center.y));
	}

	if (getMarkers())
	for (int x = 0; x < 2; x++)
	for (int y = 0; y < 2; y++)
	{
		graphics_draw_pixel(ctx, GPoint(center.x - 01 + x, center.y + 18 + y));
		graphics_draw_pixel(ctx, GPoint(center.x - 13 + x, center.y + 13 + y));
		graphics_draw_pixel(ctx, GPoint(center.x + 13 + x, center.y + 13 + y));
	}
}

void day_display_layer_update_callback(Layer *me, GContext* ctx) {
	time_t now = time(NULL);
	struct tm *t = localtime(&now);

	int daysInMonth = t->tm_mon == 3 || t->tm_mon == 5 || t->tm_mon == 8 || t->tm_mon == 10 ? 30 :
	t->tm_mon != 1 ? 31:
	t->tm_year % 4 == 0 && (t->tm_year % 400 == 0 || t->tm_year % 100 != 0) ? 29 : 28;

	unsigned int angle = (t->tm_mday * 180) / daysInMonth;

	GRect bounds = layer_get_bounds(me);
	GPoint center = grect_center_point(&bounds);
	center.y += 10;

	graphics_context_set_stroke_color(ctx, GColorWhite);

	graphics_draw_arc(ctx, center, 62, 10, 0, angle);

	if (getEndlines()) {
		graphics_draw_line(ctx, GPoint(27, center.y), GPoint(50, center.y));
		graphics_draw_line(ctx, GPoint(94, center.y), GPoint(117, center.y));
	}

	if (getMarkers())
	for (int x = -1; x < 3; x++)
	for (int y = -1; y < 3; y++)
		if (x + y > -2 && x + y < 4 && x - y < 3 && y - x < 3)
		{
			graphics_draw_pixel(ctx, GPoint(center.x - 00 + x, center.y + 66 + y));
			graphics_draw_pixel(ctx, GPoint(center.x - 47 + x, center.y + 47 + y));
			graphics_draw_pixel(ctx, GPoint(center.x + 47 + x, center.y + 47 + y));
		}

	if (getMarkers())
	for (int x = 0; x < 2; x++)
	for (int y = 0; y < 2; y++)
	{
		graphics_draw_pixel(ctx, GPoint(center.x - 64 + x, center.y + 17 + y));
		graphics_draw_pixel(ctx, GPoint(center.x - 57 + x, center.y + 33 + y));
		graphics_draw_pixel(ctx, GPoint(center.x - 33 + x, center.y + 57 + y));
		graphics_draw_pixel(ctx, GPoint(center.x - 17 + x, center.y + 64 + y));

		graphics_draw_pixel(ctx, GPoint(center.x + 64 + x, center.y + 17 + y));
		graphics_draw_pixel(ctx, GPoint(center.x + 57 + x, center.y + 33 + y));
		graphics_draw_pixel(ctx, GPoint(center.x + 33 + x, center.y + 57 + y));
		graphics_draw_pixel(ctx, GPoint(center.x + 17 + x, center.y + 64 + y));
	}
}

void hour_display_layer_update_callback(Layer *me, GContext* ctx) {
	time_t now = time(NULL);
	struct tm *t = localtime(&now);

	unsigned int angle = (t->tm_hour % 12) * 15 + 180;

	GRect bounds = layer_get_bounds(me);
	GPoint center = grect_center_point(&bounds);
	center.y -= 10;

	graphics_context_set_stroke_color(ctx, GColorWhite);

	graphics_draw_arc(ctx, center, 62, 10, 180, angle);

	if (getEndlines()) {
		graphics_draw_line(ctx, GPoint(7, center.y), GPoint(20, center.y));
		graphics_draw_line(ctx, GPoint(124, center.y), GPoint(137, center.y));
	}

	if (getMarkers())
	for (int x = -1; x < 3; x++)
	for (int y = -1; y < 3; y++)
		if (x + y > -2 && x + y < 4 && x - y < 3 && y - x < 3)
		{
			graphics_draw_pixel(ctx, GPoint(center.x - 00 + x, center.y - 66 + y));
			graphics_draw_pixel(ctx, GPoint(center.x - 47 + x, center.y - 47 + y));
			graphics_draw_pixel(ctx, GPoint(center.x + 47 + x, center.y - 47 + y));
		}

	if (getMarkers())
	for (int x = 0; x < 2; x++)
	for (int y = 0; y < 2; y++)
	{
		graphics_draw_pixel(ctx, GPoint(center.x - 64 + x, center.y - 17 + y));
		graphics_draw_pixel(ctx, GPoint(center.x - 57 + x, center.y - 33 + y));
		graphics_draw_pixel(ctx, GPoint(center.x - 33 + x, center.y - 57 + y));
		graphics_draw_pixel(ctx, GPoint(center.x - 17 + x, center.y - 64 + y));

		graphics_draw_pixel(ctx, GPoint(center.x + 64 + x, center.y - 17 + y));
		graphics_draw_pixel(ctx, GPoint(center.x + 57 + x, center.y - 33 + y));
		graphics_draw_pixel(ctx, GPoint(center.x + 33 + x, center.y - 57 + y));
		graphics_draw_pixel(ctx, GPoint(center.x + 17 + x, center.y - 64 + y));
	}
}

void minute_display_layer_update_callback(Layer *me, GContext* ctx) {
	time_t now = time(NULL);
	struct tm *t = localtime(&now);

	unsigned int angle = t->tm_min * 3 + 180;

	GRect bounds = layer_get_bounds(me);
	GPoint center = grect_center_point(&bounds);
	center.y -= 10;

	graphics_context_set_stroke_color(ctx, GColorWhite);

	graphics_draw_arc(ctx, center, 42, 20, 180, angle);

	if (getEndlines()) {
		graphics_draw_line(ctx, GPoint(27, center.y), GPoint(50, center.y));
		graphics_draw_line(ctx, GPoint(94, center.y), GPoint(117, center.y));
	}

	if (getMarkers())
	for (int x = 0; x < 2; x++)
	for (int y = 0; y < 2; y++)
	{
		graphics_draw_pixel(ctx, GPoint(center.x - 01 + x, center.y - 18 + y));
		graphics_draw_pixel(ctx, GPoint(center.x - 13 + x, center.y - 13 + y));
		graphics_draw_pixel(ctx, GPoint(center.x + 13 + x, center.y - 13 + y));
	}
}

void update_text(struct tm *tick_time) {
	static char date_str[] = "xxx 00";
	static char time_str[] = "00 00";

	char *time_format;
	if (clock_is_24h_style()) {
		time_format = "%H %M";
	} else {
		time_format = "%I %M";
	}

	strftime(time_str, sizeof(time_str), time_format, tick_time);
	strftime(date_str, sizeof(date_str), "%b %d", tick_time);

	text_layer_set_text(date_text_layer, date_str);
	text_layer_set_text(time_text_layer, time_str);
}

void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
	if ((units_changed & MONTH_UNIT) != 0)
		layer_mark_dirty(month_display_layer);
	if ((units_changed & DAY_UNIT) != 0)
		layer_mark_dirty(day_display_layer);
	if ((units_changed & HOUR_UNIT) != 0)
		layer_mark_dirty(hour_display_layer);
	if ((units_changed & MINUTE_UNIT) != 0)
		layer_mark_dirty(minute_display_layer);
	
	update_text(tick_time);
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
	autoconfig_in_received_handler(iter, context);

	time_t now = time(NULL);
	struct tm *tick_time = localtime(&now);

	handle_tick(tick_time, MONTH_UNIT);
	handle_tick(tick_time, DAY_UNIT);
	handle_tick(tick_time, HOUR_UNIT);
	handle_tick(tick_time, MINUTE_UNIT);
}

void init(void) {
	autoconfig_init();

	app_message_register_inbox_received(in_received_handler);

	window = window_create();
	window_stack_push(window, true /* Animated */);
	window_set_background_color(window, GColorBlack);

	Layer *windowLayer = window_get_root_layer(window);
	GRect windowFrame = layer_get_frame(windowLayer);

	month_display_layer = layer_create(windowFrame);
	layer_set_update_proc(month_display_layer, month_display_layer_update_callback);
	layer_add_child(windowLayer, month_display_layer);

	day_display_layer = layer_create(windowFrame);
	layer_set_update_proc(day_display_layer, day_display_layer_update_callback);
	layer_add_child(windowLayer, day_display_layer);

	hour_display_layer = layer_create(windowFrame);
	layer_set_update_proc(hour_display_layer, hour_display_layer_update_callback);
	layer_add_child(windowLayer, hour_display_layer);

	minute_display_layer = layer_create(windowFrame);
	layer_set_update_proc(minute_display_layer, minute_display_layer_update_callback);
	layer_add_child(windowLayer, minute_display_layer);

	date_text_layer = text_layer_create(GRect(82,168/2-12,62,24));
	text_layer_set_text_color(date_text_layer, GColorWhite);
	text_layer_set_background_color(date_text_layer, GColorClear);
	text_layer_set_text_alignment(date_text_layer, GTextAlignmentCenter);
	text_layer_set_font(date_text_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTOCONDENSED_LIGHT_18)));
	layer_add_child(windowLayer, text_layer_get_layer(date_text_layer));

	time_text_layer = text_layer_create(GRect(0,168/2-13,62,26));
	text_layer_set_text_color(time_text_layer, GColorWhite);
	text_layer_set_background_color(time_text_layer, GColorClear);
	text_layer_set_text_alignment(time_text_layer, GTextAlignmentCenter);
	text_layer_set_font(time_text_layer, fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ROBOTOCONDENSED_BOLD_20)));
	layer_add_child(windowLayer, text_layer_get_layer(time_text_layer));

	time_t now = time(NULL);
	struct tm *tick_time = localtime(&now);

	handle_tick(tick_time, MONTH_UNIT);
	handle_tick(tick_time, DAY_UNIT);
	handle_tick(tick_time, HOUR_UNIT);
	handle_tick(tick_time, MINUTE_UNIT);

	tick_timer_service_subscribe(MINUTE_UNIT, handle_tick);
}

void deinit(void) {
	tick_timer_service_unsubscribe();

	layer_remove_child_layers(window_get_root_layer(window));

	layer_destroy(month_display_layer);
	layer_destroy(day_display_layer);
	layer_destroy(hour_display_layer);
	layer_destroy(minute_display_layer);

	text_layer_destroy(date_text_layer);
	text_layer_destroy(time_text_layer);

	window_destroy(window);

	autoconfig_deinit();
}

int main(void) {
	init();
	app_event_loop();
	deinit();
}


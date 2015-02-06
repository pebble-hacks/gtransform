#include <pebble.h>

#include "gtransform.h"

#define DEG_TO_TRIG_ANGLE(angle) (((angle % 360) * TRIG_MAX_ANGLE) / 360)

static Window *window;
static Layer *s_canvas;
static AppTimer *s_render_timer;

#define MAX_SCALE 1.5
#define MIN_SCALE 0.5
static float s_scale_factor;
static bool s_scale_up;
static uint8_t s_scale_pause_count;

static uint8_t s_seconds_index;
static uint8_t s_seconds_count;

static GPoint s_center;

static int16_t s_sun_distance;
static int16_t s_sun_radius;

static int32_t s_earth_angle;
static int32_t s_earth_radius;
static int16_t s_earth_distance;  // distance from sun to earth

static int32_t s_moon_angle;
static int32_t s_moon_radius;
static int16_t s_moon_distance;   // distance from earth to moon

#define FRAME_RATE 20

#define SUN_DIST_OFFSET 0
#define SUN_RADIUS 30

#define EARTH_ANGLE_OFFSET (-(DEG_TO_TRIG_ANGLE(90 / FRAME_RATE)))
#define EARTH_RADIUS 10
#define EARTH_DIST_OFFSET (SUN_RADIUS + EARTH_RADIUS + 30)

#define MOON_ANGLE_OFFSET (-(DEG_TO_TRIG_ANGLE(180 / FRAME_RATE)))
#define MOON_RADIUS 4
#define MOON_DIST_OFFSET (EARTH_RADIUS + MOON_RADIUS + 10)

#define NUM_STARS 60
static const GPoint stars[NUM_STARS] = {
  {  2,   2},
  { 30,  10},
  { 60,   6},
  { 90,  12},
  {120,   8},
  { 17,  17},
  { 45,  25},
  { 76,  21},
  {105,  27},
  {135,  23},
  {  2,  32},
  { 30,  40},
  { 60,  36},
  { 90,  42},
  {120,  38},
  { 17,  47},
  { 45,  55},
  { 76,  51},
  {105,  57},
  {135,  53},
  {  2,  62},
  { 30,  70},
  { 60,  66},
  { 90,  72},
  {120,  68},
  { 17,  77},
  { 45,  85},
  { 76,  81},
  {105,  87},
  {135,  83},
  {  2,  92},
  { 30, 100},
  { 60,  96},
  { 90, 102},
  {120,  98},
  { 17, 107},
  { 45, 115},
  { 76, 111},
  {105, 117},
  {135, 113},
  {  2, 122},
  { 30, 130},
  { 60, 126},
  { 90, 132},
  {120, 128},
  { 17, 137},
  { 45, 145},
  { 76, 141},
  {105, 147},
  {135, 143},
  {  2, 152},
  { 30, 160},
  { 60, 156},
  { 90, 162},
  {120, 158},
  { 17, 167},
  { 45, 165},
  { 76, 161},
  {105, 167},
  {135, 163},
};

static void draw_star_background(GContext *ctx) {
  GTransform ts = GTransformScaleFromNumber(s_scale_factor + MIN_SCALE, s_scale_factor + MIN_SCALE);

  graphics_context_set_stroke_color(ctx, GColorWhite);
  for (int index = 0; index < NUM_STARS; index++) {
    if (index % 3 != (s_seconds_index % 3)) {
      graphics_draw_pixel(ctx, GPointFromGPointPrecise(gpoint_transform(stars[index], &ts)));
    }
  }
}

static void frame_handler(GContext *ctx) {
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, layer_get_bounds(window_get_root_layer(window)), 0, GCornerNone);

  if (s_scale_pause_count < FRAME_RATE * 2) {
    s_scale_pause_count++;
  }
  else {
    s_scale_pause_count = FRAME_RATE * 2;
    s_scale_factor += s_scale_up ? 0.05 : -0.05;

    // Pause for two seconds after each switch of scale sign
    if (s_scale_factor >= MAX_SCALE) {
      s_scale_up = false;
      s_scale_pause_count = 0;
    } else if (s_scale_factor <= MIN_SCALE) {
      s_scale_up = true;
      s_scale_pause_count = 0;
    }
  }

  draw_star_background(ctx);

  // Transform sun size and position
  GTransform ts = GTransformScaleFromNumber(s_scale_factor, s_scale_factor);
  GTransform tt = GTransformTranslationFromNumber(s_center.x, s_center.y);
  GVector sun_vector = GVector(0, SUN_RADIUS);
  sun_vector = GVectorFromGVectorPrecise(gvector_transform(sun_vector, &ts));
  s_sun_radius = sun_vector.dy;
  GPoint sun_point = GPoint(0, s_sun_distance);
  sun_point = GPointFromGPointPrecise(gpoint_transform(sun_point, &tt));
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, sun_point, s_sun_radius);

  // Transform earth size and position
  GVector earth_vector = GVector(0, EARTH_RADIUS);
  earth_vector = GVectorFromGVectorPrecise(gvector_transform(earth_vector, &ts));
  s_earth_radius = earth_vector.dy;
  earth_vector = GVector(0, EARTH_DIST_OFFSET);
  earth_vector = GVectorFromGVectorPrecise(gvector_transform(earth_vector, &ts));
  s_earth_distance = earth_vector.dy;

  // Rotate earth position
  GPoint earth_point = GPoint(0, -s_earth_distance);
  s_earth_angle = (s_earth_angle + EARTH_ANGLE_OFFSET) % TRIG_MAX_ANGLE;
  GTransform tr = GTransformRotation(s_earth_angle);
  GTransform t_concat;
  gtransform_concat(&t_concat, &tr, &tt);
  earth_point = GPointFromGPointPrecise(gpoint_transform(earth_point, &t_concat));
  graphics_fill_circle(ctx, earth_point, s_earth_radius);

  // Transform moon size and position
  GVector moon_vector = GVector(0, MOON_RADIUS);
  moon_vector = GVectorFromGVectorPrecise(gvector_transform(moon_vector, &ts));
  s_moon_radius = moon_vector.dy;
  moon_vector = GVector(0, MOON_DIST_OFFSET);
  moon_vector = GVectorFromGVectorPrecise(gvector_transform(moon_vector, &ts));
  s_moon_distance = moon_vector.dy;

  // Rotate moon position - translate by the center of the earth
  GPoint moon_point = GPoint(0, -s_moon_distance);
  s_moon_angle = (s_moon_angle + MOON_ANGLE_OFFSET) % TRIG_MAX_ANGLE;
  tr = GTransformRotation(s_moon_angle);
  tt = GTransformTranslationFromNumber(earth_point.x, earth_point.y);
  gtransform_concat(&t_concat, &tr, &tt);
  moon_point = GPointFromGPointPrecise(gpoint_transform(moon_point, &t_concat));
  graphics_fill_circle(ctx, moon_point, s_moon_radius);

  // Draw lines and circles between each of the center points for visual reference
  graphics_draw_circle(ctx, earth_point, s_moon_distance);
  graphics_draw_circle(ctx, sun_point, s_earth_distance);
  graphics_context_set_stroke_color(ctx, GColorWhite);
  graphics_draw_line(ctx, sun_point, earth_point);
  graphics_draw_line(ctx, earth_point, moon_point);
}

static void frame_timer_handler(void *context) {
  layer_mark_dirty(s_canvas);

  // Next frame
  s_render_timer = app_timer_register(1000 / FRAME_RATE, frame_timer_handler, NULL);
}

static void draw_frame_update_proc(Layer *layer, GContext *ctx) {
  frame_handler(ctx);
}

static void time_handler(struct tm *tick_time, TimeUnits units_changed) {
  // Update the star background every 7 seconds
  if (++s_seconds_count > 6) {
    s_seconds_index = (s_seconds_index + 1) % 3;
    s_seconds_count = 0;
  }
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect window_bounds = layer_get_bounds(window_layer);
  s_canvas = layer_create(GRect(0, 0, window_bounds.size.w, window_bounds.size.h));
  layer_set_update_proc(s_canvas, draw_frame_update_proc);
  layer_add_child(window_layer, s_canvas);

  s_render_timer = app_timer_register(1000 / FRAME_RATE, frame_timer_handler, NULL);
  tick_timer_service_subscribe(SECOND_UNIT, time_handler);
}

static void window_unload(Window *window) {
  layer_destroy(s_canvas);
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);

  Layer *window_layer = window_get_root_layer(window);
  GRect frame = layer_get_frame(window_layer);
  s_center = grect_center_point(&frame);

  s_scale_factor = 1;
  s_scale_up = true;
  s_scale_pause_count = 0;

  s_seconds_index = 0;
  s_seconds_count = 0;

  s_sun_distance = SUN_DIST_OFFSET;
  s_sun_radius = SUN_RADIUS;

  s_earth_angle = 0;
  s_earth_distance = EARTH_DIST_OFFSET;
  s_earth_radius = EARTH_RADIUS;

  s_moon_angle = 0;
  s_moon_distance = MOON_DIST_OFFSET;
  s_moon_radius = MOON_RADIUS;
}

static void deinit(void) {
  if(s_render_timer != NULL) {
    // Cancel any Timer
    app_timer_cancel(s_render_timer);
    s_render_timer = NULL;
  }

  tick_timer_service_unsubscribe();

  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}

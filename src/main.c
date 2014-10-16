#include <pebble.h>
#include <raytracer.h>

static Window *window;
static Layer *layer;

static GContext *layer_ctx;

byte raw_data[W * H] = {};
byte result[W * H] = {};

static void layer_update_proc(struct Layer *layer, GContext *ctx){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "layer_update_proc");

    layer_ctx = ctx;

    // Clear
    graphics_context_set_fill_color(ctx, GColorWhite);
    graphics_fill_rect(ctx, (GRect){{0, 0}, {W, H}}, 0, GCornerNone);

    //graphics_context_set_stroke_color(ctx, GColorClear);
    //graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_context_set_stroke_color(layer_ctx, GColorBlack);

    for (int y=0; y<H; y++) {
        for(int x=0; x<W; x++){
            if(result[y*W+x]==0){
                graphics_draw_pixel(layer_ctx, (GPoint){.x=x, .y=y});
            }
        }
    }
}

static void draw_raytracer() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "start draw_raytracer");

    create_image(raw_data, result);

    layer_mark_dirty(layer);

    APP_LOG(APP_LOG_LEVEL_DEBUG, "end draw_raytracer");
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
    draw_raytracer();
}

static void config_provider(Window *window) {
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);

    layer = layer_create((GRect) { .origin = { 0, 0}, .size = { 144, 168 } });
    layer_set_update_proc(layer, layer_update_proc);
    layer_add_child(window_layer, layer);
}

static void window_unload(Window *window) {
    layer_destroy(layer);
}

static void init(void) {
    window = window_create();
    window_set_click_config_provider(window, (ClickConfigProvider) config_provider);
    window_set_window_handlers(window, (WindowHandlers) {
            .load = window_load,
            .unload = window_unload,
            });
    const bool animated = true;
    window_stack_push(window, animated);
}

static void deinit(void) {
    window_destroy(window);
}

int main(void) {
    init();

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

    app_event_loop();

    deinit();
}

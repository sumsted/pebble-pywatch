#include <pebble.h>

static Window *s_main_window;
static TextLayer *s_time_layer, *s_date_layer;
static BitmapLayer *s_background_layer;
static GBitmap *s_background_bitmap;

static void update_time_and_date(){
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    static char time_buffer[] = "00:00";
    if(clock_is_24h_style() == true){
        strftime(time_buffer, sizeof(time_buffer), "%H:%M", tick_time);
    }else{
        strftime(time_buffer, sizeof(time_buffer), "%I:%M", tick_time);
    }
    if(time_buffer[0] == '0'){
        memmove(time_buffer, &time_buffer[1], sizeof(time_buffer)-1);
    }    
    text_layer_set_text(s_time_layer, time_buffer);


    static char date_buffer[] = "September 16";
    static char day_buffer[] = "16";
    static char month_buffer[] = "September";
    strftime(month_buffer, sizeof(month_buffer), "%B", tick_time);
    strftime(day_buffer, sizeof(day_buffer), "%d", tick_time);
    if(day_buffer[0] == '0'){
        memmove(day_buffer, &day_buffer[1], sizeof(day_buffer)-1);
    }
    
    memcpy(date_buffer, month_buffer, strlen(month_buffer)+1);
    memcpy(date_buffer+strlen(month_buffer), " ", 1);
    memcpy(date_buffer+strlen(month_buffer)+1, day_buffer, strlen(day_buffer)+1);
    text_layer_set_text(s_date_layer, date_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed){
    update_time_and_date();
}

static void main_window_load(Window *window){
    s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
    s_background_layer = bitmap_layer_create(GRect(0, 0, 144, 154));
    bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
    layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_background_layer));
    
    s_time_layer = text_layer_create(GRect(2, 112, 142, 50));
    text_layer_set_background_color(s_time_layer, GColorClear);
    text_layer_set_text_color(s_time_layer, GColorWhite);
    text_layer_set_text(s_time_layer, "");
    text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_time_layer));

    s_date_layer = text_layer_create(GRect(2, 3, 142, 25));
    text_layer_set_background_color(s_date_layer, GColorClear);
    text_layer_set_text_color(s_date_layer, GColorWhite);
    text_layer_set_text(s_date_layer, "");
    text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
    text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_date_layer));
}

static void main_window_unload(Window *window){
    text_layer_destroy(s_time_layer);
    text_layer_destroy(s_date_layer);
    gbitmap_destroy(s_background_bitmap);
    bitmap_layer_destroy(s_background_layer);
}

static void init() {
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });
    window_set_background_color(s_main_window, GColorBlack);
    window_stack_push(s_main_window, true);
    update_time_and_date();
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
    window_destroy(s_main_window);
    tick_timer_service_unsubscribe();
}


int main(void) {
    init();
    app_event_loop();
    deinit();
}
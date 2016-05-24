#include "hazy.h"
#include "ui.h"
#include "helpers.h"
#include "config.h"

int PWIDTH = 144;
int HALFPWIDTH = 72;
int PHEIGHT = 168;
int HALFPHEIGHT = 84;

void ui_set_datetime(struct tm *tick_time, TimeUnits units_changed) {
    if (units_changed & MINUTE_UNIT) {
        strftime(ui.texts.time, sizeof(ui.texts.time), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);

        //Remove leading 0
        if (ui.texts.time[0] == '0') {
            memmove(ui.texts.time, &ui.texts.time[1], sizeof(ui.texts.time) - 1);
        }

        text_layer_set_text(ui.layers.time, ui.texts.time);
    }

    if (units_changed & DAY_UNIT || units_changed & MONTH_UNIT) {
        strftime(ui.texts.date, sizeof(ui.texts.date), "%a, %b %e", tick_time);

        text_layer_set_text(ui.layers.date, ui.texts.date);
    }
}

void ui_set_aqi(int aqi) {
    if (aqi == -999) { //Error condition
        strncpy(ui.texts.aqi, "", sizeof(ui.texts.aqi));
    }
    else {
        snprintf(ui.texts.aqi, sizeof(ui.texts.aqi), "%d", aqi);
    }

    text_layer_set_text(ui.layers.aqi, ui.texts.aqi);

    #ifdef PBL_COLOR
    if (aqi >= 0 && aqi <= 50) {
        window_set_background_color(ui.window, GColorGreen);
    }
    else if (aqi > 50 && aqi <= 100) {
        window_set_background_color(ui.window, GColorYellow);
    }
    else if (aqi > 100 && aqi <= 150) {
        window_set_background_color(ui.window, GColorOrange);
    }
    else if (aqi > 150 && aqi <= 200) {
        window_set_background_color(ui.window, GColorRed);
    }
    else if (aqi > 200 && aqi <= 300) {
        window_set_background_color(ui.window, GColorJazzberryJam);
    }
    else if (aqi > 300) {
        window_set_background_color(ui.window, GColorDarkCandyAppleRed);
    }
    else {
        window_set_background_color(ui.window, GColorDarkGray);
    }
    #endif
}

void ui_window_load(Window *window) {
    ui.layers.window = window_get_root_layer(window);

    #ifdef PBL_COLOR
    window_set_background_color(ui.window, GColorDarkGray);
    #endif

    //Dynamically set sizes
    GRect window_bounds = layer_get_bounds(ui.layers.window);
    PWIDTH = window_bounds.size.w;
    HALFPWIDTH = window_bounds.size.w / 2;
    PHEIGHT = window_bounds.size.h;
    HALFPHEIGHT = window_bounds.size.h / 2;

    int width = PWIDTH * 3 / 4;
    int margin_left = PWIDTH / 8;
    int margin_top = PHEIGHT / 4;
    int border_width = 4;

    #ifdef PBL_ROUND
        width = PWIDTH * 5 / 8;
        margin_left = PWIDTH * 3 / 16;
        margin_top = PHEIGHT  * 5 / 16;
    #endif

    ui.layers.date = text_layer_init(
        ui.layers.window,
        GRect(margin_left - border_width, margin_top - 25, width + (border_width * 2), 30),
        ui.fonts.droidsans_15,
        GColorClear,
        GColorBlack,
        GTextAlignmentLeft
    );

    ui.layers.aqi_border = bitmap_layer_init(
        ui.layers.window,
        GRect(margin_left - border_width, margin_top - border_width, width + (border_width * 2), 73),
        NULL,
        GColorWhite
    );

    ui.layers.aqi = text_layer_init(
        ui.layers.window,
        GRect(margin_left, margin_top, width, 65),
        ui.fonts.droidsans_bold_50,
        GColorBlack,
        GColorWhite,
        GTextAlignmentCenter
    );

    ui.layers.time = text_layer_init(
        ui.layers.window,
        GRect(margin_left - border_width, margin_top + 65, width + (border_width * 2), 40),
        ui.fonts.droidsans_bold_30,
        GColorClear,
        GColorBlack,
        GTextAlignmentRight
    );

    #ifdef PBL_ROUND
        text_layer_set_text_alignment(ui.layers.date, GTextAlignmentCenter);
        text_layer_set_text_alignment(ui.layers.time, GTextAlignmentCenter);
    #endif

    text_layer_show(ui.layers.date);
    text_layer_show(ui.layers.aqi);
    bitmap_layer_show(ui.layers.aqi_border);
    text_layer_show(ui.layers.time);
}

void ui_window_unload(Window *window) {
    text_layer_destroy_safe(ui.layers.date);
    text_layer_destroy_safe(ui.layers.aqi);
    bitmap_layer_destroy_safe(ui.layers.aqi_border);
    text_layer_destroy_safe(ui.layers.time);
}

void ui_init() {
    struct Fonts fonts;
    struct Texts texts;
    struct Layers layers;

    fonts.droidsans_bold_50 = fonts_load_resource_font(RESOURCE_ID_DROIDSANS_BOLD_50);
    fonts.droidsans_bold_30 = fonts_load_resource_font(RESOURCE_ID_DROIDSANS_BOLD_30);
    fonts.droidsans_15 = fonts_load_resource_font(RESOURCE_ID_DROIDSANS_15);

    ui.layers = layers;
    ui.texts = texts;
    ui.fonts = fonts;

    strncpy(ui.texts.time, "", sizeof(ui.texts.time));
    strncpy(ui.texts.aqi, "", sizeof(ui.texts.aqi));
    strncpy(ui.texts.date, "", sizeof(ui.texts.date));

    ui.window = window_create();
    window_set_window_handlers(ui.window, (WindowHandlers) {
        .load = ui_window_load,
        .unload = ui_window_unload,
    });

    const bool animated = true;
    window_stack_push(ui.window, animated);
}

void ui_deinit() {
    window_destroy(ui.window);

    fonts_unload_custom_font_safe(ui.fonts.droidsans_bold_50);
    fonts_unload_custom_font_safe(ui.fonts.droidsans_bold_30);
    fonts_unload_custom_font_safe(ui.fonts.droidsans_15);
}

#pragma once

#include <pebble.h>

struct Fonts {
    GFont droidsans_bold_45;
    GFont droidsans_bold_30;
    GFont droidsans_15;
};

struct Texts {
    char time[6];
    char aqi[5];
    char date[15];
};

struct Layers {
    Layer *window;
    TextLayer *time;
    TextLayer *aqi;
    BitmapLayer *aqi_border;
    TextLayer *date;
};

struct UI {
    Window *window;
    struct Layers layers;
    struct Fonts fonts;
    struct Texts texts;
};

void ui_layout();
void ui_set_datetime(struct tm *tick_time, TimeUnits units_changed);
void ui_set_aqi(int aqi);
void ui_set_unobstructed_area(GRect unobstructed_area);

void ui_init();
void ui_deinit();

struct UI ui;

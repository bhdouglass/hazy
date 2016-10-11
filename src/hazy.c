#include <pebble.h>

#include "hazy.h"
#include "helpers.h"
#include "config.h"
#include "ui.h"

int elapsed_time = 0;
int error_wait = 0;
bool bluetooth_connected = true;
int fails = 0;
int error = NO_ERROR;

static void fetch() {
    if (bluetooth_connected) {
        Tuplet value = TupletInteger(APP_KEY_FETCH, 1);
        DictionaryIterator *iter;
        app_message_outbox_begin(&iter);

        if (iter == NULL) {
            return;
        }

        dict_write_tuplet(iter, &value);
        dict_write_end(iter);
        app_message_outbox_send();
        //AppMessageResult send_result = app_message_outbox_send();
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "%s", translate_message_error(send_result));

        error = FETCH_ERROR;
    }
}

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
    ui_set_datetime(tick_time, units_changed);

    elapsed_time++;
    if (error == NO_ERROR) {
        if (elapsed_time >= config.refresh_time) {
            fetch();
        }
    }
    else {
        if (elapsed_time >= error_wait) {
            fetch();
        }
    }
}

static void handle_outbox_failed(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "message failed to send: %s", translate_message_error(reason));

    ui_set_aqi(NO_DATA);

    fails++;
    if (fails < 10) {
        fetch();
    }
}

static void handle_inbox_received(DictionaryIterator *iter, void *context) {
    fails = 0;

    bool config_updated = false;

    Tuple *data = dict_read_first(iter);
    while(data != NULL) {
        switch(data->key) {
            case APP_KEY_AQI:
                ui_set_aqi(data->value->int32);
                elapsed_time = 0;

                break;

            case APP_KEY_AQI_ERROR:
                error = data->value->int32;

                if (error == NO_ERROR) {
                    error_wait = 0;
                }
                else {
                    if (error_wait < 10) {
                        error_wait++;
                    }
                }

                break;

            default:
                config_recieved(data);
                config_updated = true;

                break;
        }

        data = dict_read_next(iter);
    }

    if (config_updated) {
        config_save();
    }
}

/*static void handle_inbox_dropped(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "message dropped: %s", translate_message_error(reason));
}*/

/*static void handle_outbox_sent(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "message sent sucessfully");
}*/

static void handle_bluetooth(bool connected) {
    bluetooth_connected = connected;

    if (elapsed_time >= config.refresh_time) {
        fetch();
    }
}

static void handle_obstruction(GRect final_unobstructed_screen_area, void *context) {
    //TODO smartly handle this, rather than just hide stuff

    ui_set_unobstructed_area(final_unobstructed_screen_area);
}

static void init(void) {
    setlocale(LC_ALL, "");

    config_init();
    ui_init();

    time_t now = time(NULL);
    struct tm *tick_time = localtime(&now);
    handle_tick(tick_time, MINUTE_UNIT | DAY_UNIT | MONTH_UNIT);
    tick_timer_service_subscribe(MINUTE_UNIT, &handle_tick);

    handle_bluetooth(connection_service_peek_pebble_app_connection());
    bluetooth_connection_service_subscribe(&handle_bluetooth);

    UnobstructedAreaHandlers unobstructed_area_handlers = {
        .will_change = handle_obstruction,
    };
    unobstructed_area_service_subscribe(unobstructed_area_handlers, NULL);

    app_message_register_outbox_failed(&handle_outbox_failed);
    app_message_register_inbox_received(&handle_inbox_received);
    //app_message_register_inbox_dropped(&handle_inbox_dropped);
    //app_message_register_outbox_sent(&handle_outbox_sent);
    app_message_open(640, 64);
}

static void deinit(void) {
    ui_deinit();

    tick_timer_service_unsubscribe();
    bluetooth_connection_service_unsubscribe();
    unobstructed_area_service_unsubscribe();
    app_message_deregister_callbacks();
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}

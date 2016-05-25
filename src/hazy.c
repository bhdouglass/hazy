#include <pebble.h>
#include "kiezelpay.h"

#include "hazy.h"
#include "helpers.h"
#include "config.h"
#include "ui.h"

int elapsed_time = 0;
bool bluetooth_connected = true;
int fails = 0;

//TODO wait in the event of an error & remove aqi
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
    }
}

static void handle_tick(struct tm *tick_time, TimeUnits units_changed) {
    ui_set_datetime(tick_time, units_changed);

    elapsed_time++;
    if (elapsed_time >= config.refresh_time) {
        fetch();
    }
}

static void handle_outbox_failed(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "message failed to send: %s", translate_message_error(reason));

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


static void init(void) {
    setlocale(LC_ALL, "");

    kiezelpay_init();
    config_init();
    ui_init();

    time_t now = time(NULL);
    struct tm *tick_time = localtime(&now);
    handle_tick(tick_time, MINUTE_UNIT | DAY_UNIT | MONTH_UNIT);
    tick_timer_service_subscribe(MINUTE_UNIT, &handle_tick);

    handle_bluetooth(connection_service_peek_pebble_app_connection());
    bluetooth_connection_service_subscribe(&handle_bluetooth);

    kiezelpay_settings.on_inbox_received = handle_inbox_received;
    //kiezelpay_settings.on_inbox_dropped = handle_inbox_dropped;
    kiezelpay_settings.on_outbox_failed = handle_outbox_failed;
    //kiezelpay_settings.on_outbox_sent = handle_outbox_sent;

    //app_message_register_outbox_failed(&handle_outbox_failed);
    //app_message_register_inbox_received(&handle_inbox_received);
    //app_message_register_inbox_dropped(&handle_inbox_dropped);
    //app_message_register_outbox_sent(&handle_outbox_sent);
    //app_message_open(640, 64);
}

static void deinit(void) {
    ui_deinit();
    kiezelpay_deinit();

    tick_timer_service_unsubscribe();
    bluetooth_connection_service_unsubscribe();
    //app_message_deregister_callbacks();
}

int main(void) {
    init();
    app_event_loop();
    deinit();
}

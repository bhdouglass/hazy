#include <pebble.h>

#include "hazy.h"
#include "config.h"

void config_init() {
<% _.forEach(config_types, function(data, name) { %>
    <% if (_.isObject(data.default) && data.type == 'string') { %>
    #ifdef PBL_PLATFORM_APLITE
        strncpy(config.<%= name %>, <%= data.default.aplite %>, sizeof(config.<%= name %>));
    #endif
    #ifdef PBL_PLATFORM_BASALT
        strncpy(config.<%= name %>, <%= data.default.basalt %>, sizeof(config.<%= name %>));
    #endif
    #ifdef PBL_PLATFORM_CHALK
        strncpy(config.<%= name %>, <%= data.default.chalk %>, sizeof(config.<%= name %>));
    #endif
    <% } else if (_.isObject(data.default) && data.type != 'string') { %>
    #ifdef PBL_PLATFORM_APLITE
        config.<%= name %> = <%= data.default.aplite %>;
    #endif
    #ifdef PBL_PLATFORM_BASALT
        config.<%= name %> = <%= data.default.basalt %>;
    #endif
    #ifdef PBL_PLATFORM_CHALK
        config.<%= name %> = <%= data.default.chalk %>;
    #endif
    <% } else if (data.type == 'string') { %>
    strncpy(config.<%= name %>, <%= data.default %>, sizeof(config.<%= name %>));
    <% } else { %>
    config.<%= name %> = <%= data.default %>;
    <% } %>
<% }); %>

<% _.forEach(config_types, function(data, name) { %>
    if (persist_exists(APP_KEY_<%= data.uppercase %>)) {
        <% if (data.type == 'string') { %>
        persist_read_string(APP_KEY_<%= data.uppercase %>, config.<%= name %>, <%= data.length %>);
        <% } else { %>
        config.<%= name %> = persist_read_<%= data.type %>(APP_KEY_<%= data.uppercase %>);
        <% } %>
    }
<% }); %>
}

void config_save() {
<% _.forEach(config_types, function(data, name) { %>
    persist_write_<%= data.type %>(APP_KEY_<%= data.uppercase %>, config.<%= name %>);
<% }); %>
}

void config_recieved(Tuple *data) {
    switch(data->key) {
<% _.forEach(config_types, function(data, name) { %>
        case APP_KEY_<%= data.uppercase %>:
            <% if (data.type == 'string') { %>
            strncpy(config.<%= name %>, data->value-><%= data.message_type %>, sizeof(config.<%= name %>));
            <% } else { %>
            config.<%= name %> = data->value-><%= data.message_type %>;
            <% } %>
            break;
<% }); %>
    }
}

#pragma once

<% _.forEach(config_constants, function(value, name) { %>
#define CONFIG_<%= name %> <%= value %>
<% }); %>

struct Config {
<% _.forEach(config_types, function(data, name) { %>
    <% if (data.type == 'string') { %>
    char <%= name %>[<%= data.length %>];
    <% } else { %>
    <%= data.ctype %> <%= name %>;
    <% } %>
<% }); %>
};

void config_init();
void config_save();
void config_recieved(Tuple *data);

struct Config config;

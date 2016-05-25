#pragma once

#define NO_ERROR 0
#define LOCATION_ERROR 1
#define AQI_ERROR 2
#define FETCH_ERROR 3

#define NO_DATA -999

<% _.forEach(app_keys, function(value, name) { %>
#define APP_KEY_<%= name %> <%= value %>
<% }); %>

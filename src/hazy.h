#pragma once

<% _.forEach(app_keys, function(value, name) { %>
#define APP_KEY_<%= name %> <%= value %>
<% }); %>

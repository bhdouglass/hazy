var datalog = '';
var last_status_code = -1;
var last_location_error_code = -1;

module.exports = {
    APP_START: 'a',
    MESSAGE_RECIEVED: 'b',
    CONFIGURATION: 'c',
    CONFIGURATION_CANCELED: 'd',
    CONFIGURATION_CLOSE: 'e',
    FETCH_AQI: 'f',
    AQI_SUCCESS: 'g',
    AQI_ERROR: 'h',
    FETCH_LOCATION: 'i',
    LOCATION_SUCCESS: 'j',
    LOCATION_ERROR: 'k',

    log: function(msg) {
        if (datalog.length > 100) {
            datalog = datalog.substring(10, datalog.length);
        }

        datalog += msg;

        window.localStorage.setItem('datalog', datalog);
    },

    getLog: function() {
        return datalog;
    },

    load: function() {
        datalog = window.localStorage.getItem('datalog');
        if (!datalog) {
            datalog = '';
        }
    },

    setStatusCode: function(code) {
        last_status_code = code;
    },

    getStatusCode: function() {
        return last_status_code;
    },

    setLocationErrorCode: function(code) {
        last_location_error_code = code;
    },

    getLocationErrorCode: function() {
        return last_location_error_code;
    },
};

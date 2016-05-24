var config = require('config');
var logger = require('logger');
var metadata = require('metadata');
var constants = require('constants');

var WeatherMan = require('libs/weather-man');
var MessageQueue = require('libs/js-message-queue');

function ack(e) {
    console.log('Successfully delivered message with transactionId=' + e.data.transactionId);
}

function nack(e) {
    console.log('Unable to deliver message with transactionId=' + e.data.transactionId + ', error is: ' + e.error.message);
}

Pebble.addEventListener('ready', function(e) {
    console.log('starting js, v' + metadata.versionLabel);

    logger.load();
    logger.log(logger.APP_START);

    config.load();
    config.send();

    //MessageQueue.sendAppMessage({aqi: 40}, ack, nack); //for nice screenshots
    fetchLocation();
});

Pebble.addEventListener('appmessage', function(e) {
    logger.log(logger.MESSSAGE_RECIEVED);
    console.log('Received message: ' + JSON.stringify(e.payload));
    if (e.payload && e.payload.fetch) {
        fetchLocation();
    }
});

Pebble.addEventListener('showConfiguration', function(e) {
    logger.log(logger.CONFIGURATION);

    var platform = 'aplite';
    if (Pebble.getActiveWatchInfo) {
        platform = Pebble.getActiveWatchInfo().platform;
    }

    var url = '<%= config_url %>?platform=' +
        platform + '&version=' + metadata.versionLabel +
        '&dl=' + logger.getLog() + '&lsc=' + logger.getStatusCode() +
        '&llec=' + logger.getLocationErrorCode() +
        '#' + encodeURIComponent(JSON.stringify(config.config));

    Pebble.openURL(url);
    console.log(url);
});

Pebble.addEventListener('webviewclosed', function(e) {
    if (!e.response || e.response == 'cancel') {
        logger.log(logger.CONFIGURATION_CANCELED);
        console.log('config canceled');
    }
    else {
        logger.log(logger.CONFIGURATION_CLOSE);

        config.save(JSON.parse(decodeURIComponent(e.response)));
        config.send();
    }
});

function fetchLocation() {
    logger.log(logger.FETCH_LOCATION);

    window.navigator.geolocation.getCurrentPosition(function(pos) { //Success
        logger.log(logger.LOCATION_SUCCESS);

        console.log('lat: ' + pos.coords.latitude);
        console.log('lng: ' + pos.coords.longitude);

        fetchAirQuality(pos);

    }, function(err) { //Error
        logger.log(logger.LOCATION_ERROR);
        logger.setLocationErrorCode(err.code);

        console.warn('location error: ' + err.code + ' - ' + err.message);

    }, { //Options
        timeout: 30000, //30 seconds
        maximumAge: 300000, //5 minutes
    });
}

function fetchAirQuality(pos) {
    var message = {
        aqi: -999,
    };

    logger.log(logger.FETCH_AQI);

    var wm = new WeatherMan(WeatherMan.AQICN);
    wm.getCurrent(pos.coords.latitude, pos.coords.longitude).then(function(result) {
        logger.log(logger.AQI_SUCCESS);

        config.config.last_location = result.getLocation();
        config.save();

        message.aqi = result.getAQI();
        MessageQueue.sendAppMessage(message, ack, nack);

    }).catch(function(result) {
        logger.log(logger.AQI_ERROR);
        console.warn('aqi error: ' + JSON.stringify(result));

        message.aqi = -999;

        if (result && result.status) {
            console.log(result.status);
            logger.setStatusCode(result.status);
        }

        MessageQueue.sendAppMessage(message, ack, nack);
    });
}

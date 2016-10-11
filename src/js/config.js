var metadata = require('./metadata');
var MessageQueue = require('./libs/js-message-queue');

function Config() {
    this.config = {};
}

Config.prototype.send = function() {
    var message = {};
    for (var index in metadata.config) {
        var meta = metadata.config[index];

        if (!meta.only) {
            message[meta.name] = this.config[meta.name];
        }
    }

    MessageQueue.sendAppMessage(message, function ack(e) {
        console.log('Successfully delivered config (transactionId=' + e.data.transactionId + ')');
    }, function nack(e) {
        var message = e.error ? e.error.message : 'unknown';
        console.log('Unable to deliver config (transactionId=' + e.data.transactionId + '), error: ' + message);
    });
};

Config.prototype.load = function() {
    var platform = 'aplite';
    if (Pebble.getActiveWatchInfo && Pebble.getActiveWatchInfo()) {
        platform = Pebble.getActiveWatchInfo().platform;
    }

    //Set defaults
    for (var index in metadata.config) {
        var meta = metadata.config[index];

        if (meta.only != 'pebble' && meta.only != 'app_key') {
            if (meta.type == 'enum') {
                this.config[meta.name] = metadata.enums[meta['enum']][meta['default']];
            }
            else {
                if (typeof meta['default'] == 'object' && meta['default'] !== null) {
                    this.config[meta.name] = (meta['default'][platform] === undefined) ? null : meta['default'][platform];
                }
                else {
                    this.config[meta.name] = (meta['default'] === undefined) ? null : meta['default'];
                }
            }
        }
    }

    //Load data from localStorage
    for (var cindex in metadata.config) {
        var cmeta = metadata.config[cindex];


        if (cmeta.only != 'pebble' && cmeta.only != 'app_key') {
            var value = window.localStorage.getItem(cmeta.name, null);
            if (value !== null) {
                if (cmeta.type == 'int' || cmeta.type == 'enum') {
                    this.config[cmeta.name] = parseInt(value);
                }
                else if (cmeta.type == 'bool') {
                    this.config[cmeta.name] = (value == '1' || value == 'true');
                }
                else if (cmeta.type == 'obj') {
                    try {
                        this.config[cmeta.name] = JSON.parse(value);
                    }
                    catch (err) {
                        this.config[cmeta.name] = {};
                    }
                }
                else {
                    this.config[cmeta.name] = value;
                }
            }
        }
    }
};

Config.prototype.save = function() {
    var types = {};
    for (var index in metadata.config) {
        var meta = metadata.config[index];

        types[meta.name] = meta.type;
    }

    for (var key in this.config) {
        var value = this.config[key];
        if (types[key] == 'obj') {
            value = JSON.stringify(value);
        }

        window.localStorage.setItem(key, value);
    }
};

module.exports = new Config();

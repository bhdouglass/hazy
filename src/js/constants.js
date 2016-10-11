var metadata = require('./metadata');

var constants = {
    NO_ERROR: 0,
    LOCATION_ERROR: 1,
    AQI_ERROR: 2,
    NO_DATA: -999,
};

for (var enum_ in metadata.enums) {
    var e = metadata.enums[enum_];
    for (var key in e) {
        var constant_key = key
            .replace(/-/g, '')
            .replace(/'/g, '')
            .replace(/\./g, '')
            .replace(/\s\s+/g, ' ')
            .trim()
            .replace(/ /g, '_')
            .toUpperCase();
        constants[enum_.toUpperCase() + '_' +constant_key] = e[key];
    }
}

module.exports = constants;

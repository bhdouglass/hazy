var metadata = require('metadata');

var constants = {};
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

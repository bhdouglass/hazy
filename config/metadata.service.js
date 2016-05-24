'use strict';

angular.module('app').factory('metadata', function() {
    function constant(name) {
        return name
            .replace(/-/g, '')
            .replace(/'/g, '')
            .replace(/\./g, '')
            .replace(/\//g, '')
            .replace(/\s\s+/g, ' ')
            .trim()
            .replace(/ /g, '_')
            .toUpperCase();
    }

    var metadata = <%= metadata %>;
    metadata.inject_enums = function(scope) {
        scope.constants = {};
        for (var name in metadata.enums) {
            var e = metadata.enums[name];

            var constant_name = constant(name);
            var enum_ = [];
            for (var key in e) {
                scope.constants[constant_name + '_' + constant(key)] = e[key];

                enum_.push({
                    label: key,
                    value: e[key],
                });
            }

            scope[name] = enum_;
        }
    };

    return metadata;
});

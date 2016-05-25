'use strict';

angular.module('app').controller('indexCtrl', function($scope, $http, $location, $timeout, metadata) {
    metadata.inject_enums($scope);

    $scope.loaded = false;
    $scope.saving = false;
    $scope.version = 1.0;
    $scope.latestVersion = metadata.versionLabel;
    $scope.platform = 'aplite';
    $scope.token = null;
    $scope.config = {};
    $scope.debug = {
        config: null,
        version: null,
        datalog: '',
        last_status_code: -1,
        last_location_error_code: -1,
        platform: '',
    };

    $scope.cancel = function() {
        if ($scope.saving) {
            return;
        }

        window.location.href = 'pebblejs://close#cancel';
    };

    $scope.save = function() {
        if ($scope.saving) {
            return;
        }

        $scope.saving = true;
        var config = angular.copy($scope.config);
        for (var index in metadata.config) {
            var meta = metadata.config[index];

            if (meta.type == 'int') {
                config[meta.name] = Math.abs(parseInt(config[meta.name]));
            }
        }

        console.log(config);
        window.location.href = 'pebblejs://close#' + encodeURIComponent(JSON.stringify(config));
    };

    $timeout(function() {
        var query = $location.search();
        if (query.version) {
            $scope.version = parseFloat(query.version);
        }

        if (query.platform) {
            $scope.platform = query.platform;
        }

        if (query.dl) {
            $scope.debug.datalog = query.dl;
        }

        if (query.lsc) {
            $scope.debug.last_status_code = query.lsc;
        }

        if (query.llec) {
            $scope.debug.last_location_error_code = query.llec;
        }

        if (query.token) {
            $scope.token = query.token;
        }

        console.log('version: ' + $scope.version);
        console.log('lastest version: ' + $scope.latestVersion);

        var hash = $location.hash();
        if (hash) {
            var config = JSON.parse(decodeURIComponent(hash));

            for (var index in metadata.config) {
                var meta = metadata.config[index];

                if (meta.only != 'pebble' && meta.only != 'app_key') {
                    $scope.config[meta.name] = (config[meta.name] === undefined) ? null : config[meta.name];
                }
            }

            if ($scope.config.refresh_time <= 0) {
                $scope.config.refresh_time = 1;
            }

            console.log($scope.config);
        }

        $scope.debug.config = $scope.config;
        $scope.debug.version = $scope.version;
        $scope.debug.platform = $scope.platform;

        $scope.loaded = true;
    });
});

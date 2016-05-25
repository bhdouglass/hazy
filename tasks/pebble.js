var metadata = require('../metadata.json');
var gulp = require('gulp');
var template = require('gulp-template');
var shell = require('gulp-shell');
var file = require('gulp-file');
var rename = require("gulp-rename");
var del = require('del');
var minimist = require('minimist');
var fs = require('fs');

var paths = {
    js: ['src/js/*.js', 'src/js/libs/*.js'],
    js_dist: 'dist/pebble/src/js/',
    js_base: 'src/js',
    c: ['src/*.h', 'src/*.c', 'wscript'],
    resources: 'resources/**/*',
    c_dist: 'dist/pebble/',
    pbw: 'dist/pebble/build/pebble.pbw',
    dist: 'dist/',
};

var args = minimist(process.argv.slice(2), {
    default: {
        emulator: false,
        aplite: false,
        basalt: true,
        chalk: false,
        phone: '192.168.1.21',
        logs: false,
        debug: false,
        config: 'http://' + fs.readFileSync('CNAME', 'utf-8').trim() + '/',
    },
    boolean: ['emulator', 'aplite', 'basalt', 'chalk'],
    alias: {
        emulator: ['e', 'emu'],
        logs: 'l',
        debug: 'd',
    }
});

args.phone = process.env.PHONE ? process.env.PHONE : args.phone;

function installCommand(args) {
    var command = 'pebble install';
    if (args.emulator) {
        command += ' --emulator';

        if (args.aplite) {
            command += ' aplite';
        }
        else if (args.chalk) {
            command += ' chalk';
        }
        else {
            command += ' basalt';
        }
    }
    else {
        command += ' --phone ' + args.phone;
    }

    if (args.logs) {
        command += ' --logs';
    }

    if (args.debug) {
        command += ' --debug';
    }

    return command;
}

gulp.task('pebble-clean', function() {
    del.sync(paths.c_dist);
});

gulp.task('pebble-build-js', function() {
    return gulp.src(paths.js, {base: paths.js_base})
        .pipe(template({
            config_url: args.config,
            metadata: JSON.stringify(metadata),
        }))
        .pipe(gulp.dest(paths.js_dist));
});

gulp.task('pebble-build-c', function() {
    var message_types = {
        'int': 'int32',
        string: 'cstring',
        bool: 'int32',
        'enum': 'int32',
    };

    var ctypes = {
        'int': 'int',
        string: 'char*',
        bool: 'bool',
        'enum': 'int',
    };

    var app_keys = {};
    var config_constants = {};
    var config_types = {};

    for (var index in metadata.config) {
        var meta = metadata.config[index];

        if (meta.only == 'app_key') {
            if (meta.app_key !== undefined) {
                app_keys[meta.name.toUpperCase()] = meta.app_key;
            }
        }
        else if (meta.only != 'js') {
            var config_type = {
                type: meta.type,
                ctype: ctypes[meta.type],
                message_type: message_types[meta.type],
                uppercase: meta.name.toUpperCase(),
                length: meta.length,
            };

            if (meta.app_key !== undefined) {
                app_keys[meta.name.toUpperCase()] = meta.app_key;
            }

            if (meta.type == 'enum') {
                var enum_value = metadata.enums[meta['enum']][meta['default']];
                config_type['default'] = enum_value;
            }
            else if (meta.type == 'string') {
                config_type['default'] = '"' + meta['default'] + '"';
            }
            else {
                config_type['default'] = meta['default'];
            }

            config_types[meta.name] = config_type;
        }
    }

    for (var name in metadata.enums) {
        var e = metadata.enums[name];

        for (var ekey in e) {
            var constant_key = ekey
                .replace(/-/g, '')
                .replace(/'/g, '')
                .replace(/\./g, '')
                .replace(/\//g, '')
                .replace(/\s\s+/g, ' ')
                .trim()
                .replace(/ /g, '_')
                .toUpperCase();

            config_constants[name.toUpperCase() + '_' + constant_key] = e[ekey];
        }
    }

    return gulp.src(paths.c, {base: '.'})
        .pipe(template({
            metadata: JSON.stringify(metadata),
            app_keys: app_keys,
            config_constants: config_constants,
            config_types: config_types,
        }))
        .pipe(gulp.dest(paths.c_dist));
});

gulp.task('pebble-build-resources', function() {
    return gulp.src(paths.resources, {base: '.'})
        .pipe(gulp.dest(paths.c_dist));
});

gulp.task('pebble-build-appinfo', function() {
    var app_keys = metadata.extra_app_keys ? metadata.extra_app_keys : {};
    var config_constants = {};
    var config_types = {};

    for (var index in metadata.config) {
        var meta = metadata.config[index];

        if (meta.only != 'js') {
            if (meta.app_key !== undefined) {
                app_keys[meta.name] = meta.app_key;
            }
        }
    }

    var capabilities= ['configurable'];
    if (metadata.health) {
        capabilities.push('health');
    }

    var appinfo = {
        uuid: metadata.uuid,
        shortName: metadata.name,
        longName: metadata.longName ? metadata.longName : metadata.name,
        companyName: metadata.author,
        versionLabel: metadata.versionLabel,
        sdkVersion: metadata.sdkVersion ? metadata.sdkVersion : "3",
        targetPlatforms: metadata.targetPlatforms ? metadata.targetPlatforms : ['aplite', 'basalt', 'chalk'],
        enableMultiJS: !!metadata.enableMultiJS,
        watchapp: {
            watchface: !!metadata.watchface,
        },
        appKeys: app_keys,
        resources: metadata.resources,
        capabilities: capabilities,
    };

    return file('appinfo.json', JSON.stringify(appinfo, null, 4), {src: true})
        .pipe(gulp.dest(paths.c_dist));
});

gulp.task('pebble-build-src', ['lint', 'pebble-build-appinfo', 'pebble-clean', 'pebble-build-resources', 'pebble-build-c', 'pebble-build-js']);
gulp.task('pebble-prebuild', ['pebble-build-src'], shell.task(['cd ' + paths.c_dist + ' && pebble build']));
gulp.task('pebble-build', ['pebble-prebuild'], function() {
    var filename = metadata.name.toLowerCase().replace(/ /g, '-') +
        '_' + metadata.versionLabel + '.pbw';

    return gulp.src(paths.pbw)
        .pipe(rename(filename))
        .pipe(gulp.dest(paths.dist));
});
gulp.task('pebble-install', ['pebble-build'], shell.task(['cd ' + paths.c_dist + ' && ' + installCommand(args)]));

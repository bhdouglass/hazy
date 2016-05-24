var gulp = require('gulp');
var jshint = require('gulp-jshint');
var stylish = require('jshint-stylish');

require('./tasks/config');
require('./tasks/pebble');

var paths = {
    jslint: ['gulpfile.js', 'tasks/*.js', 'src/js/*.js', '!src/js/metadata.js', 'config/*.js', '!config/metadata.service.js'],
};

gulp.task('lint', function() {
    return gulp.src(paths.jslint)
        .pipe(jshint())
        .pipe(jshint.reporter(stylish))
        .pipe(jshint.reporter('fail'));
});

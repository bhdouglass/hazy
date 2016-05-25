var metadata = require('../metadata.json');
var gulp = require('gulp');
var uglify = require('gulp-uglify');
var concat = require('gulp-concat');
var template = require('gulp-template');
var connect = require('gulp-connect');
var gopen = require('gulp-open');
var ngAnnotate = require('gulp-ng-annotate');
var sourcemaps = require('gulp-sourcemaps');
var htmlmin = require('gulp-htmlmin');
var minifyCSS = require('gulp-minify-css');
var surge = require('gulp-surge');
var del = require('del');
var fs = require('fs');

var paths = {
    html: 'config/*.html',
    js: [
        'config/bower_components/Slate/dist/js/slate.min.js',
        'config/bower_components/angular/angular.min.js',
        'config/bower_components/angular-slate/dist/js/angular-slate.min.js',
        'config/*.js',
    ],
    css: [
        'config/bower_components/Slate/dist/css/slate.min.css',
        'config/bower_components/angular-slate/dist/css/angular-slate.min.css',
        'config/*.css',
    ],
    fonts: [
        'config/bower_components/Slate/dist/fonts/PFDinDisplayPro-Light.woff',
        'config/bower_components/Slate/dist/fonts/PTSans-regular.woff',
    ],
    font: [], //For fonts that don't use the fonts convention (so we don't have to rewrite the csss)
    dist: 'dist/config/',
};

gulp.task('config-clean', function() {
    del.sync(paths.dist);
});

gulp.task('config-build-js', function() {
    return gulp.src(paths.js)
        .pipe(template({
            metadata: JSON.stringify(metadata),
        }))
        .pipe(sourcemaps.init())
        .pipe(concat('app.js'))
        .pipe(ngAnnotate())
        .pipe(uglify())
        .pipe(sourcemaps.write())
        .pipe(gulp.dest(paths.dist));
});

gulp.task('config-build-html', function() {
    return gulp.src(paths.html)
        .pipe(template({
            metadata: metadata,
        }))
        .pipe(htmlmin({collapseWhitespace: true}))
        .pipe(gulp.dest(paths.dist));
});

gulp.task('config-build-css', function() {
    return gulp.src(paths.css)
        .pipe(concat('main.css'))
        .pipe(minifyCSS())
        .pipe(gulp.dest(paths.dist));
});

gulp.task('config-build-fonts', function() {
    return gulp.src(paths.fonts)
        .pipe(gulp.dest(paths.dist + 'fonts'));
});

gulp.task('config-build-font', function() {
    return gulp.src(paths.font)
        .pipe(gulp.dest(paths.dist + 'font'));
});

gulp.task('config-watch', function() {
    gulp.watch(paths.js, ['lint', 'config-build-js']);
    gulp.watch(paths.html, ['config-build-html']);
    gulp.watch(paths.css, ['config-build-css']);
    gulp.watch(paths.fonts, ['config-build-fonts']);
});

gulp.task('config-build', [
    'lint',
    'config-clean',
    'config-build-html',
    'config-build-js',
    'config-build-css',
    'config-build-fonts',
    'config-build-font'
]);

gulp.task('config-deploy', ['config-build'], function() {
    return surge({
        project: paths.dist,
        domain: fs.readFileSync('CNAME', 'utf-8'),
    });
});

gulp.task('config-deploy-dev', ['config-build'], function() {
    return surge({
        project: paths.dist,
        domain: 'dev-' + fs.readFileSync('CNAME', 'utf-8'),
    });
});

gulp.task('serve', ['config-build', 'config-watch'], function() {
    connect.server({
        root: 'dist/config',
        ip: '0.0.0.0',
        port: 9000,
    });

    return gulp.src(paths.html)
        .pipe(gopen({
            uri: 'http://localhost:9000?platform=basalt&version=' + metadata.versionLabel
        }));
});

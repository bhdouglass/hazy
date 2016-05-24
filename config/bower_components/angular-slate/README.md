# Angular Slate

Angular bindings for the [Pebble Slate](https://github.com/pebble/slate) framework.

Checkout the demo at [angular-slate.bhdouglass.com](http://angular-slate.bhdouglass.com/).

## Download

Get it via bower: `bower install --save angular-slate`

OR download the `dist/css/angular-slate.min.css` and `dist/js/angular-slate.min.js`
files.

## Usage

Include both angular-slate.min.js and angular-slate.min.css files in your project
and add `angular-slate` to your angular app's dependencies.

## Components

### Color Picker

```
<color-picker ng-model="color" platform="{{platform}}"></color-picker>
```

- `color` is an integer representation of a Pebble color.
- `platform` is either aplite/basalt/chalk (more colors will show when it's set to basalt or chalk).

### List

```
<list ng-model="dynamic_list"></list>
```

- `dynamic_list` is an array of strings that can be removed or added to by the user.

### Draggable List

```
<draggable ng-model="draggable_list" label="label"></draggable>
```

- `draggable_list` is an array of objects that will be reordered by the user.
- `label` is a property of `draggable_list` that will be shown to the user.

## License

Copyright (C) 2016 [Brian Douglass](http://bhdouglass.com/)

This program is free software: you can redistribute it and/or modify it under
the terms of the GNU General Public License version 3, as published by the Free
Software Foundation.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranties of MERCHANTABILITY, SATISFACTORY
QUALITY, or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License along with
this program.  If not, see <http://www.gnu.org/licenses/>.

# qarkdown

A [Qt](http://qt.nokia.com/products/) [markdown](http://daringfireball.net/projects/markdown/) preview app.

## Features

Edit text in an editor on the left side of the app and see a preview of the rendered markdown/html output on the right. The app is kept decidedly simple. You can load and save files. The window title displays the currently opened file.

### Keyboard shortcuts

- `CTRL-U` displays a source view.
- `CTRL-O` opens a file
- `CTRL-S` saves the file

## Dependencies

- Qt 4.7.4 or higher (not tested with older versions)
- [sundown-0.1](https://github.com/sschober/sundown)

## Building

The usual

    qmake
    make

should do it.

## Feature Ideas

- Customizable UI - Configurable editor position and size, e.g. fixed column size
- Versioning - maybe based on `git`
- Simple file switching - maybe enabled by a little file browser on the left side of the app
- Some kind of journal support - Ability to add a new entry/file/whatever tagged by the current date and time just by hitting a single key combo (`CTRL-N`)

## Author

Sven schober

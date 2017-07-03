# qarkdown

A [markdown](http://daringfireball.net/projects/markdown/) editor sporting a nice instant preview and syntax highlighting.

It is written in [Qt](http://qt.nokia.com/products/). It can handle UTF-8 encoded texts, so nice [box drawings](http://en.wikipedia.org/wiki/Box-drawing_character) like theses are possible:

    ┌────────┐    has    ┌────────────────┐
    │ Person │──────────>│ Role           │
    └────────┘      1..n ├────────────────┤
        │                │+Expire : Date  │
    has │                └────────────────┘
        │1..m                     │
        v                         │
    ┌────────────────┐            │
    │ Subscription   │ 1..l       │ implies
    ├────────────────┤ <──────────┘
    │+Expire : Date  │
    └────────────────┘

## Features

Edit text in an editor on the left side of the app and see a preview of the rendered markdown/html output on the right. The app is kept decidedly simple. You can load and save files. The window title displays the currently opened file.

Markdown text is syntax highlighted using Ali Rantakari's nice [PEG Markdown Highlighter](http://hasseg.org/peg-markdown-highlight/).

### Keyboard shortcuts

- `CTRL-U` displays a source view.
- `CTRL-O` opens a file
- `CTRL-S` saves the file
- `CTRL-E` exports the file as HTML (replacing a `md[k]` suffix by `html`)
- `CTRL-N` start a fresh file (pressing `CTRL-S` asks for file name)
- `CTRL-D` opens a directory view on the left, which shows the contents of the parent folder of the currently opened file.
- Pressing `TAB` on selected Text indents it by 4 characters (handy for code listings).

### Details 

The rendered markdown content is wrapped in a `HTML`-Document, which includes a custom [`bootstrap.css`](http://twitter.github.com/bootstrap/) style sheet. The only thing i added to the style sheet is horizontal columnization, so the rendered content grows columnwise to the right. I modified/customized `QWebView` to scroll horizontally on mouse wheel events (see my struggles [here](http://stackoverflow.com/questions/10721118/let-qwebview-scroll-horizontally-on-mouse-wheel/10754333#10754333)).

On changing the currently opened file using the directory view (which happens by using a _single_ left click) the current file is saved without further enquiries. This might be unexpected for you...

## Dependencies

- Qt 5.9.1 (no longer tested with older versions)

## Building

The usual

    qmake
    make

should do it.

## Invoking

    qarkdown [<file>]

If `<file>` is given it is opened and displayd when it exists. Otherwise it is created.

## Feature Ideas

- Customizable UI - Configurable editor position and size, e.g. fixed column size
- Versioning - maybe based on `git`
- Some kind of journal support - Ability to add a new entry/file/whatever tagged by the current date and time just by hitting a single key combo (`CTRL-N`)

## Authors

* [Sven Schober](mailto:sschober@sssm.de)
* [pasnox](mailto:pasnox@gmail.com)

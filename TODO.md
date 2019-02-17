# To Do

## Bug Fixes

* Relative paths don't work on the Linux command line (issue [#17](https://github.com/Rangi42/polished-map/issues/17))
* Resizing window resets sidebar width to original zoom size (sometimes)
* Ctrl+C/V/X when editing lighting interferes with regular cut/copy/paste somehow (worked around for now)

## Features

* Native-looking build on Mac OS X (issue [#8](https://github.com/Rangi42/polished-map/issues/8); involves publishing an app bundle release, and using the system menu bar)
* Scale the UI for high-DPI displays
* Generate map images from the command line (issue [#10](https://github.com/Rangi42/polished-map/issues/10))
* Parse, display, and edit event scripts (only editing the coordinates) ([view/edit UI concept](https://i.imgur.com/YwtFzlZ.png))
* Allow undo/redo for resize operations
* Show a mini-palette of hotkeyed blocks, which the scroll wheel can cycle through
* File → Open Recent → the last few opened .blk files (issue [#11](https://github.com/Rangi42/polished-map/issues/11))

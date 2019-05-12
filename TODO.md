# To Do

## Bug Fixes

* Resizing window resets sidebar width to original zoom size (sometimes)
* Ctrl+C/V/X when editing lighting interferes with regular cut/copy/paste somehow (worked around for now)
* Projects with `PRIORITY` colors but not 256 tiles still write `rept 16` to palette_map.asm

## Features

* Native-looking build on Mac OS X (issue [#8](https://github.com/Rangi42/polished-map/issues/8); involves publishing an app bundle release, and using the system menu bar)
* Scale the UI for high-DPI displays
* Generate map images from the command line (issue [#10](https://github.com/Rangi42/polished-map/issues/10))
* Edit and save map events
* Allow undo/redo for resize operations
* Show a mini-palette of hotkeyed blocks, which the scroll wheel can cycle through
* File → Open Recent → the last few opened .blk files (issue [#11](https://github.com/Rangi42/polished-map/issues/11))

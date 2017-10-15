# To Do

* **Fix:** Resizing window resets sidebar width to original zoom size (sometimes)


## Maybe

* Allow undo/redo for resize operations and/or block edits
* Choose to put the sidebar on the left or right
* Choose to display coordinates as X/Y and WxH or Y/X and HxW
* More color schemes than just Day, Nite, and Indoor
* Change color schemes for an already-open map
* Swap tilesets for an already-open map
* Use IFileOpenDialog, not SHBrowseForFolder, for new maps (Windows only)


## Never

* Automatically find map data for .blk
   * A .blk file could have zero or many associated .asm files, not just one; and multiple maps sharing a .blk could have different sizes or tilesets. (Polished Crystal already has examples of these cases.)
* Edit associated event data
   * Even if a .blk had one associated map, the cost-benefit of writing an event editor program instead of using a text editor is not worth it.
   * I dislike when crowdmap messes up .asm files' formatting and creates spurious changelogs.
* Edit associated collision data
   * Polished Crystal's collision data is stored as assembly code, not binary, and does not need a GUI editor.

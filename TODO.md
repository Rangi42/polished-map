# To Do

* Undo + Redo
* Add/remove metatiles
* Save default settings to registry


## Maybe

* Swap tilesets for an already-open map
* Change color schemes for an already-open map
* More color schemes than just Day, Nite, and Indoor


## Never

* Automatically find map data for .blk
   * A .blk file could have zero or many associated .asm files, not just one; and multiple maps sharing a .blk could have different sizes or tilesets. (Polished Crystal already has examples of these cases.)
* Edit associated event data
   * Even if a .blk had one associated map, the cost-benefit of writing an event editor program instead of using a text editor is not worth it.
   * I specifically dislike when Crowdmap messes up .asm files' formatting and creates spurious changelogs.

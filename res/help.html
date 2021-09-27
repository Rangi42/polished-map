R"(<html>
<head>
<title>)" PROGRAM_NAME R"( Help</title>
</head>
<body>
<h1>)" PROGRAM_NAME R"(</h1>
<p>Copyright © )" CURRENT_YEAR " " PROGRAM_AUTHOR R"(</p>
<p>A map and tileset editor for <a href="https://github.com/pret/pokecrystal">pokecrystal</a>, <a href="https://github.com/pret/pokegold">pokegold</a>, <a href="https://github.com/pret/pokeyellow">pokeyellow</a>, <a href="https://github.com/pret/pokered">pokered</a>, and custom projects based on them.</p>
<p>Inspired by <a href="https://github.com/yenatch/crowdmap">crowdmap</a> (now defunct), but implemented with C++ and <a href="http://www.fltk.org/">FLTK</a>, and with more functions for graphics editing.</p>
<p>Source code is available at <a href="https://github.com/Rangi42/polished-map">https://github.com/Rangi42/polished-map</a> under the <a href="https://github.com/Rangi42/polished-map/blob/master/LICENSE.md">LGPL v3.0</a> license.</p>
<hr>
<p>)" PROGRAM_NAME R"( displays and edits .blk map files. It supports all the standard ways to open a file:</p>
<ul>
<li><b>Initial:</b> Double-click a .blk file in File Explorer. (Run install.bat to associate .blk files with )" PROGRAM_NAME R"(.)</li>
<li><b>Menu bar:</b> Run )" PROGRAM_EXE R"( and select the File&nbsp;→&nbsp;Open… menu item.</li>
<li><b>Toolbar:</b> Run )" PROGRAM_EXE R"( and click the Open toolbar button.</li>
<li><b>Keyboard:</b> Run )" PROGRAM_EXE R"( and press Ctrl+O.</li>
<li><b>File Explorer:</b> Drag a .blk file onto )" PROGRAM_EXE R"(.</li>
<li><b>GUI:</b> Drag a .blk file onto an open )" PROGRAM_NAME R"( window.</li>
<li><b>Command Prompt:</b> Pass the .blk filename as an argument to )" PROGRAM_EXE R"(:<br><font size="2"><kbd>)" PROGRAM_EXE "&nbsp;pokecrystal" DIR_SEP "maps" DIR_SEP R"(SomeMap.blk</kbd></font></li>
</ul>
<p>You can include a map's size and tileset in its filename as <var>MapName</var>.<var>WIDTH</var>x<var>HEIGHT</var>.<var>TILESET</var>.blk; for instance, NewBarkTown.10x9.blk, UnionCave1F.cave.blk, or House1.4x4.house.blk.</p>
<p>Otherwise )" PROGRAM_NAME R"( will try to guess a map's size and tileset from the assembly code. This can fail if the map and tileset filenames do not exactly correspond to their constants in the code. Then you'll just have to enter the size and tileset yourself.</p>
<p>Be sure to set the right options (via the Options menu) before creating or opening a map. The available options are:</p>
<ul>
<li><b>Monochrome:</b> Tileset palettes will not be read or written. This option is necessary for <a href="https://github.com/pret/pokered">pokered</a>-based projects.</li>
<li><b>Tile Priority:</b> Allow tiles to have priority over sprites. <a href="https://github.com/pret/pokecrystal">pokecrystal</a>'s *_palette_map.asm files only assign a color to each tile. <a href="https://github.com/pret/pokecrystal/wiki/Allow-map-tiles-to-appear-above-sprites-(so-NPCs-can-walk-behind-tiles)-with-PRIORITY-colors">This tutorial</a> explains how to make them assign priority attributes as well. This option is necessary to correctly save the *_palette_map.asm files.</li>
<li><b>256 Tiles:</b> Allow all 256 tile IDs from $00 to $FF. <a href="https://github.com/pret/pokecrystal">pokecrystal</a>'s tileset graphics have tiles for IDs $00 to $5F, then skip $60 to $7F and continue from $80 to $DF, with $E0 to $FF unused. <a href="https://github.com/pret/pokecrystal/wiki/Expand-tilesets-from-192-to-255-tiles">This tutorial</a> explains how to use all the IDs for map tiles except $7F, which is used for spaces in text. This option is necessary for projects that support larger tilesets, such as Polished Crystal and Orange.</li>
<li><b>Roof Palettes:</b> pokecrystal loads two pairs of ROOF colors per map group by default: one for Morn + Day, one for Night. If your project is modified to use different ROOF colors (such as by <a href="https://github.com/pret/pokecrystal/wiki/Make-evening-the-fourth-time-of-day">making evening the fourth time of day</a>, with its own third pair of Custom colors), this option will let you choose which palettes they load into.</li>
<li><b>Auto-Load Events:</b> Automatically loads the .asm file corresponding to the map if one exists. For example, if you open maps)" DIR_SEP "PalletTown.blk, then this option will automatically try to load maps" DIR_SEP "PalletTown.asm (for pokecrystal-based projects) or data" DIR_SEP "maps" DIR_SEP "objects" DIR_SEP R"(PalletTown.asm (for pokered).</li>
<li><b>Auto-Load Special Palettes:</b> Automatically loads a .pal file associated with the map if one exists. Association is based on the map, landmark, or tileset. For example, if you open maps)" DIR_SEP "IcePath1F.blk with the tileset gfx" DIR_SEP "tilesets" DIR_SEP "ice_path.png and the landmark in data" DIR_SEP "maps" DIR_SEP "maps.asm for IcePath is ICE_PATH, then this option will automatically load maps" DIR_SEP "IcePath1F.pal (corresponding to the map), or else gfx" DIR_SEP "tilesets" DIR_SEP "ice_path.pal (the landmark), or else gfx" DIR_SEP "tilesets" DIR_SEP R"(ice_path.pal (the tileset). You can also use Data&nbsp;→&nbsp;Load&nbsp;Palettes… to manually load any such .pal file.</li>
<li><b>Auto-Load Roof Colors:</b> Automatically loads colors for the ROOF palette of the map's group, if the group was detected from constants)" DIR_SEP "map_constants.asm and the roof palettes are defined in gfx" DIR_SEP "tilesets" DIR_SEP "roofs.pal (or tilesets" DIR_SEP R"(roof.pal for backwards compatibility with older pokecrystal versions). You can also use Data&nbsp;→&nbsp;Load&nbsp;Roof&nbsp;Colors to do this manually.</li>
<li><b>Drag and Drop:</b> Enables drag-and-drop for blocks in the sidebar, tiles in the tileset editor or roof editor, and colors in the palette editor. Left-click and drag one item onto another to swap them; hold Ctrl and do the same to copy.</li>
<li><b>Overworld Map Size:</b> The overworld map block buffer in WRAM stores the entire current map, padded by 3 blocks on each side. )" PROGRAM_NAME R"( warns you if you save a map that would overflow this buffer. If your project modifies the buffer size, this option will let you configure the warning.</li>
</ul>
<hr>
<p>Most functions are available via the menu bar, the toolbar, or shortcut keys.</p>
<p>The sidebar array uses the mouse:</p>
<ul>
<li>Left-click to select a block.</li>
<li>Left-click and drag one block onto another to swap them.</li>
<li>Hold Ctrl, left-click and drag one block onto another to copy it.</li>
<li>Right-click to edit a block.</li>
<li>Middle-click and drag to scroll.</li>
</ul>
<p>The map area also uses the mouse, but it depends on the Mode. In Block mode (Ctrl+Shift+B or the toolbar's Headbutt tree button):</p>
<ul>
<li>Left-click or drag to place the selected block in the sidebar array.</li>
<li>Right-click to select a block from the map.</li>
<li>Middle-click and drag to scroll.</li>
<li>Hold Shift and left-click a group of blocks to flood-fill it with the selected block.</li>
<li>Hold Ctrl and left-click a block to replace every block of that type with the selected type.</li>
<li>Hold Alt and left-click a block to swap every block of that type and every block of the selected type.</li>
</ul>
<p>And in Event mode (Ctrl+Shift+T or the toolbar's Kris button):</p>
<ul>
<li>Hover over an event to see a tooltip with its full definition.</li>
<li>Left-click and drag an event to move it. You can drag into the three-block margin around a map.</li>
<li>Right-click to edit an event.</li>
<li>Double-click an event to open the .asm file in the system text editor. If you use Notepad++, Notepad2, Notepad3, Sublime Text, or VS Code on Windows, then it will also open to the specific event's line number. If you edit and save the .asm file, you can reload it in )" PROGRAM_NAME R"( with Data&nbsp;→&nbsp;Reload&nbsp;Event&nbsp;Script or Ctrl+R.</li>
<li>Middle-click and drag to scroll.</li>
<li>Hold Shift and left-click a warp event to open the map it warps to. The map's filename is guessed from the warp event's constant. For example, a warp to UNION_CAVE_B1F will try to open UnionCaveB1F.ablk.</li>
</ul>
<p>The arrow keys, or the mouse's scrolling function if it has one, will scroll the sidebar array or the map area (whichever one the cursor is over).</p>
<p>The number keys 0-9 are hotkeys for the sidebar array.</p>
<ul>
<li>Press Ctrl+0-9 to assign that key to the selected block.</li>
<li>Press 0-9 to select the block assigned to that key.</li>
<li>Press Ctrl+Shift+0-9 to unassign that key.</li>
</ul>
<p>In Block mode, you can view and edit the map's blocks. In Event mode, you can view and edit the map's events (NPCs, warps, signs, etc). Events are also visible in Block mode if View&nbsp;→&nbsp;Show&nbsp;Events is checked.</p>
<hr>
<p>The block editor window has some simple mouse controls:</p>
<ul>
<li>Left-click the tileset array to select a tile.</li>
<li>Left-click or drag inside the block area to place the selected tile in the block.</li>
<li>Right-click inside the block area to select a tile from the block.</li>
</ul>
<p>And some more complex ones for placing multiple tiles at once, with the selected tile as the top-left:</p>
<ul>
<li>Hold Ctrl and left-click to place a contiguous 2x2 square.</li>
<li>Hold Ctrl+Shift and left-click to place a contiguous 4x4 square.</li>
<li>Hold Alt and left-click to place a sequential 2+2 line in a square.</li>
<li>Hold Alt+Shift and left-click to place a sequential 4+4+4+4 line in a square.</li>
<li>Hold Ctrl+Alt and left-click to place a sequential 2-2 line in a square, with the rows going backwards.</li>
<li>Hold Ctrl+Alt+Shift and left-click to place a sequential 4-4-4-4 line in a square, with the rows going backwards.</li>
<li>Hold Shift and left-click to place two sequential tiles in a checkerboard 2/2 square.</li>
</ul>
<p>For pokecrystal-based projects, you can also edit the collision values of the block's four quadrants. Old-style collision.bin files and up-to-date collision.asm files are both supported.</p>
<hr>
<p>The tileset editor window (Ctrl+T or the toolbar's green puzzle piece button) has more mouse controls, similar to the map editor:</p>
<ul>
<li>Left-click the tileset array to select a tile.</li>
<li>Left-click and drag one tile onto another to swap them.</li>
<li>Hold Ctrl, left-click and drag one tile onto another to copy it.</li>
<li>Left-click a swatch, or press the 1/2/3/4 keys, to choose a hue.</li>
<li>Left-click or drag inside the pixel area to draw with the chosen hue in the selected tile.</li>
<li>Right-click inside the pixel area to choose a hue from the tile.</li>
<li>Hold Shift and left-click a group of pixels to flood-fill it with the selected hue.</li>
<li>Hold Ctrl and left-click a pixel to replace every pixel of that hue with the selected hue.</li>
<li>Hold Alt and left-click a pixel to swap every pixel of that hue and every pixel of the selected hue.</li>
</ul>
<p>Ctrl+C, Ctrl+V, and Ctrl+X will copy, paste, and swap tiles respectively, just like they do for blocks in the map editor's sidebar array. Delete will clear a tile's graphics and set its color to UNDEFINED.</p>
<p>The Copy and Paste buttons, or Ctrl+Shift+C and Ctrl+Shift+V, will copy and paste the tile graphics using the system clipboard. Tile graphics are always copied in monochrome grayscale, just like the tileset PNGs. Graphics can be pasted as any color; they get converted to four-hue monochrome when pasted. (So you can easily rip tiles from full-color game screenshots.)</p>
<p>Every tile has a single color. The TEXT color is meant for menus and text boxes, and should not be used for map tiles. Alt+1 through Alt+8 will set the current tile to use the <i>N</i>th palette. The "<span>Priority (above sprites)</span>" checkbox sets the PRIORITY attribute bit for a tile so it will appear above sprites, including NPCs and the player; this is <a href="https://github.com/pret/pokecrystal/wiki/Allow-map-tiles-to-appear-above-sprites-(so-NPCs-can-walk-behind-tiles)-with-PRIORITY-colors">a feature</a> of Polished Crystal and similar hacks.</p>
<hr>
<p>Tilesets can be augmented by roofs. A roof is assumed to take up the nine tiles from $0A to $12. When a roof is loaded, it visibly affects the map and the block editor, but not the tileset editor. Thus you can edit the tileset without the loaded roof overwriting those nine tiles.</p>
<p>When you open a map the roof selection always defaults to none, even if the map uses a roof. This is because pokecrystal only loads roofs for certain tilesets, which are <a href="https://github.com/pret/pokecrystal/wiki/Hard-coded-logic#tilesets-that-have-per-mapgroup-roofs">hard-coded</a> instead of being defined in a data table. This makes it impractical to automatically detect a map's roof type, so you'll have to manually specify it.</p>
<p>The roof editor window (Ctrl+F or the toolbar's green roof button) is similar to the tileset editor. However, it does not support Delete.</p>
<hr>
<p>The colors' exact appearance depends on the current palettes: MORN, DAY, NITE, INDOOR, or CUSTOM. The default set of palettes' colors is based on pokecrystal, but custom ones will be read from gfx)" DIR_SEP "tilesets" DIR_SEP "bg_tiles.pal (or tilesets" DIR_SEP R"(bg.pal for backwards compatibility with older pokecrystal versions) if it exists.</p>
<p>When a map <var>MapName</var>.blk is opened with the tileset <var>tileset</var>.png, )" PROGRAM_NAME R"( will automatically load <var>tileset</var>.pal if it exists, and <var>MapName</var>.pal if it exists. If the map is part of the <var>LANDMARK</var> landmark, it will automatically load <var>landmark</var>.pal if it exists. This is convenient for maps like Ice Path with custom palettes. This can be disabled via Options&nbsp;→&nbsp;Auto-Load&nbsp;Special&nbsp;Palettes.</p>
<p>The palette editor window (Ctrl+Shift+L or the toolbar's lightbulb button) has some mouse controls:</p>
<ul>
<li>Left-click the palette array to select a color.</li>
<li>Left-click and drag one color onto another to swap them.</li>
<li>Hold Ctrl, left-click and drag one color onto another to copy it.</li>
</ul>
<p>Ctrl+C, Ctrl+V, and Ctrl+X will copy, paste, and swap colors respectively, just like they do for blocks and tiles in their own editor dialogs.</p>
<p>You can export the edited colors of all the palettes at once to a single .pal file. Do not overwrite an existing file with this; instead, copy+paste only the palettes you need. For example, bg_tiles.pal defines MORN, DAY, NITE, and INDOOR palettes (as well as separate WATER colors for MORN/DAY and NITE), and ice_path.pal defines a CUSTOM palette.</p>
</body>
</html>)"

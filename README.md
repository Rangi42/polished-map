# Polished Map++

A map and tileset editor for [pokecrystal](https://github.com/pret/pokecrystal) hacks that allow [256 tiles](https://github.com/pret/pokecrystal/wiki/Expand-tilesets-from-192-to-255-tiles) and [per-block attributes](https://github.com/pret/pokecrystal/wiki/Allow-tiles-to-have-different-attributes-in-different-blocks-\(including-X-and-Y-flip\)), including [Polished Crystal v3](https://github.com/Rangi42/polishedcrystal), [Red++ v4](https://github.com/TheFakeMateo/RedPlusPlus), [Coral](https://github.com/pkmncoraldev/pokecoral), and [Ancient Ruby](https://github.com/BloodlessNS/ancientruby).

Inspired by [crowdmap](https://github.com/yenatch/crowdmap), but implemented with C++ and [FLTK](http://www.fltk.org/), and with more functions for graphics editing.

Latest release: [**2.3.0**](https://github.com/Rangi42/polished-map/releases/tag/v2.3.0++)

Follow the steps in [INSTALL.md](INSTALL.md) to install the release copy of Polished Map++, or the longer instructions to build it yourself from source.

The [example/](example/) directory contains a minimal pokecrystal project with two test maps. **Kanto.180x135.kanto.ablk** is a stitch of every Kanto overworld map (they all use the "kanto" tileset). **Johto.235x135.johto.ablk** is a stitch of every Johto overworld map; Goldenrod and Azalea use the johto_modern tileset, so try switching tilesets with **Edit→Change Tileset…** or by pressing Ctrl+H.

More information is at the [Skeetendo Forums](https://hax.iimarckus.org/topic/7222/). If you have questions or comments, please go there.

![Screenshot](screenshot.png)

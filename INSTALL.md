# Install Guide

## Windows

TODO: Visual Studio instructions.


## Linux

Run the following commands:

```bash
sudo apt-get install make g++ git unzip
sudo apt-get install zlib1g-dev libpng-dev libxpm-dev libx11-dev libxft-dev libxinerama-dev libfontconfig1-dev x11proto-xext-dev libxrender-dev libxfixes-dev

git clone https://github.com/roukaour/polished-map.git
cd polished-map/lib
unzip fltk-1.3.4.zip
cd fltk-1.3.4

chmod +x configure
./configure --with-abiversion=10304
sudo make
sudo make install
cp lib/*.a ../linux

cd ../..

make install
```

To build **polishedcrystal.gbc**:

```bash
make
```

To build other versions:

```bash
make [faithful] [nortc] [monochrome] [debug]
```


## Mac OS X

In **Terminal**, run:

```bash
xcode-select --install

git clone https://github.com/rednex/rgbds.git
cd rgbds
git checkout v0.3.1
sudo make install
cd ..

git clone https://github.com/roukaour/polishedcrystal.git
cd polishedcrystal
```

To build **polishedcrystal.gbc**:

```bash
make
```

To build other versions:

```bash
make [faithful] [nortc] [monochrome] [debug]
```

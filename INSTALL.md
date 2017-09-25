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
mkdir ../linux
cp lib/*.a ../linux

cd ../..

make
sudo make install
```

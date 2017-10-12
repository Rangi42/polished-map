# Install Guide

## Windows

To install Polished Map:

1. Download **install.bat** and **polishedmap.exe** from [the latest release](https://github.com/roukaour/polished-map/releases).
2. Hold down Shift, right-click **install.bat**, and click "**Run as administrator**". This will install Polished Map to your account and create a shortcut on your Desktop.

If you have Microsoft Visual Studio, you can build Polished Map yourself:

1. On GitHub, click the green "**Clone or download**" button and click "**Download ZIP**". This will download **polished-map-master.zip**.
2. Unzip polished-map-master.zip. This will create the **polished-map-master** folder.
3. Navigate to the polished-map-master folder in Explorer.
4. Unzip lib/fltk-1.3.4.zip. This will create the lib/**fltk-1.3.4** folder.
5. Open lib/fltk-1.3.4/ide/VisualC2010/fltk.sln in Visual Studio 2012. (Other versions may or may not work, I haven't tried.)
6. A dialog "Upgrade VS++ Compiler and Libraries" will open, since fltk.sln was made for Visual Studio 2008. Click OK.
7. Go to **Build → Batch Build…**, check the projects **fltk**, **fltkimages**, **fltkpng**, and **fltkzlib** in the Release configuration, and click the **Build** button.
8. Move the .lib files from lib/fltk-1.3.4/lib to lib.
9. Open ide/polished-map.sln in Visual Studio 2012.
10. If the Solution Configuration dropdown on the toolbar says Debug, set it to **Release**.
11. Go to **Build → Build Solution** or press F7 to build the project. This will create bin/Release/**polished-map.exe**.
12. Hold down Shift, right-click **install.bat**, and click "**Run as administrator**". This will install Polished Map to your account and create a shortcut on your Desktop.


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
cp lib/*.a ..

cd ../..

make
sudo make install
```

# Install Guide

## Windows

**To install Polished Map:**

1. Download **install.bat** and **polishedmap.exe** from [the latest release](https://github.com/Rangi42/polished-map/releases).
2. Hold down Shift, right-click **install.bat**, and click "**Run as administrator**". This will install Polished Map to your account and create a shortcut on your Desktop.
3. Now you can delete the two downloaded files.
4. Double-click **Polished Map** on your Desktop to open it. If you get an error that "mscvrt120.dll is missing", you need to install the [Microsoft Visual C++ 2013 Redistributable](https://www.microsoft.com/en-us/download/details.aspx?id=40784). You may need the x64 version as well as the x86 version, at least for Windows 10.

If you have Microsoft Visual Studio, you can build Polished Map yourself:

1. On GitHub, click the green "**Clone or download**" button and click "**Download ZIP**". This will download **polished-map-master.zip**.
2. Unzip polished-map-master.zip. This will create the **polished-map-master** folder.
3. Navigate to the polished-map-master folder in Explorer.
4. Unzip lib/fltk-1.3.5-mod.zip. This will create the lib/**fltk-1.3.5-mod** folder.
5. Open lib/fltk-1.3.5-mod/ide/VisualC2010/fltk.sln in Visual Studio 2013. (Other versions may or may not work, I haven't tried.)
6. A dialog "Upgrade VS++ Compiler and Libraries" will open, since fltk.sln was made for Visual Studio 2008. Click OK.
7. Go to **Build → Batch Build…**, check the projects **fltk**, **fltkimages**, **fltkpng**, **fltkjpeg**, and **fltkzlib** in the Release configuration, and click the **Build** button.
8. Move the .lib files from lib/fltk-1.3.5-mod/lib to lib.
9. Open ide/polished-map.sln in Visual Studio 2013.
10. If the Solution Configuration dropdown on the toolbar says Debug, set it to **Release**.
11. Go to **Build → Build Solution** or press F7 to build the project. This will create bin/Release/**polishedmap.exe**.
12. Hold down Shift, right-click **install.bat**, and click "**Run as administrator**". This will install Polished Map to your account and create a shortcut on your Desktop.


## Linux

Run the following commands:

```bash
# Install dependencies
# (you need at least g++ 4.9 for <regex> support)
sudo apt-get install make g++ git unzip
sudo apt-get install zlib1g-dev libpng-dev libxpm-dev libx11-dev libxft-dev libxinerama-dev libfontconfig1-dev x11proto-xext-dev libxrender-dev libxfixes-dev

# Clone Polished Map
git clone https://github.com/Rangi42/polished-map.git
cd polished-map

# Build modified FLTK 1.3.5 with the latest ABI enabled
# (even if you already have libfltk1.3-dev installed)
pushd lib
unzip fltk-1.3.5-mod.zip
cd fltk-1.3.5-mod
chmod +x configure
./configure --prefix="$PWD/.." --with-abiversion=10305
make
make install
popd

# Build Polished Map
# ("export PATH" is needed if fltk-config is not already in your PATH)
export PATH="$PWD/lib/bin:$PATH"
make

# Install Polished Map
# (tested on Ubuntu and Ubuntu derivatives only; it just copies bin/polishedmap
#  and res/app.xpm to system directories)
sudo make install
```

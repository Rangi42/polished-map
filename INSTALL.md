# Install Guide

## Windows

### Install the Polished Map release

1. Download **install.bat** and **polishedmap.exe** from [the latest release](https://github.com/Rangi42/polished-map/releases).
2. Hold down Shift, right-click **install.bat**, and click "**Run as administrator**". This will install Polished Map to your account and create a shortcut on your Desktop.
3. Now you can delete the two downloaded files.
4. Double-click **Polished Map** on your Desktop to open it. If you get an error that "msvcrt140.dll is missing", you need to install the [Microsoft Visual C++ Redistributable for Visual Studio 2019](https://www.visualstudio.com/downloads/) for x86.

### Build Polished Map from source

You will need [Microsoft Visual Studio](https://visualstudio.microsoft.com/vs/); the Community edition is free.

If the pre-built release already works for you, you don't have to build it yourself.

1. On GitHub, click the green "**Clone or download**" button and click "**Download ZIP**". This will download **polished-map-master.zip**.
2. Unzip polished-map-master.zip. This will create the **polished-map-master** folder.
3. Navigate to the polished-map-master folder in Explorer.
4. Unzip lib/fltk-1.3.5-mod.zip. This will create the lib/**fltk-1.3.5-mod** folder.
5. Open lib/fltk-1.3.5-mod/ide/VisualC2010/fltk.sln in Visual Studio 2019. (Other versions may or may not work, I haven't tried.)
6. A dialog "Upgrade VS++ Compiler and Libraries" will open, since fltk.sln was made for Visual Studio 2008. Click OK.
7. Go to **Build → Batch Build…**, check the projects **fltk**, **fltkimages**, **fltkpng**, **fltkjpeg**, and **fltkzlib** in the Release configuration, and click the **Build** button.
8. Move the .lib files from lib/fltk-1.3.5-mod/lib to lib.
9. Open ide/polished-map.sln in Visual Studio 2019.
10. If the Solution Configuration dropdown on the toolbar says Debug, set it to **Release**.
11. Go to **Build → Build Solution** or press F7 to build the project. This will create bin/Release/**polishedmap.exe**.
12. Hold down Shift, right-click **install.bat**, and click "**Run as administrator**". This will install Polished Map to your account and create a shortcut on your Desktop.


## Linux

### Install dependencies

You need at least g++ 7 for C++11 `<regex>` and C++17 `<string_view>` support.

#### Ubuntu/Debian

Run the following commands:

```bash
sudo apt-get install make g++ git unzip
sudo apt-get install zlib1g-dev libpng-dev libxpm-dev libx11-dev libxft-dev libxinerama-dev libfontconfig1-dev x11proto-xext-dev libxrender-dev libxfixes-dev
```

#### Fedora

Run the following commands:

```bash
sudo dnf install make g++ git unzip
sudo dnf install zlib-devel libpng-devel libXpm-devel libX11-devel libXft-devel libXinerama-devel fontconfig-devel libXext-devel libXrender-devel libXfixes-devel
```

### Install and build Polished Map

Run the following commands:

```bash
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

# Install Guide

## Windows

### Install the Polished Map++ release

1. Download **install.bat** and **polishedmap-plusplus.exe** from [the latest **++** release](https://github.com/Rangi42/polished-map/releases).
2. Hold down Shift, right-click **install.bat**, and click "**Run as administrator**". This will install Polished Map++ to your account and create a shortcut on your Desktop.
3. Now you can delete the two downloaded files.
4. Double-click **Polished Map++** on your Desktop to open it. If you get an error that "msvcrt140.dll is missing", you need to install the [Microsoft Visual C++ Redistributable for Visual Studio 2019](https://www.visualstudio.com/downloads/) for x86.

### Build Polished Map++ from source

You will need [Microsoft Visual Studio](https://visualstudio.microsoft.com/vs/); the Community edition is free.

If the pre-built release already works for you, you don't have to build it yourself.

1. Download the **Source code (zip)** from [the most recent **++** release](https://github.com/Rangi42/polished-map/releases). This will download **polished-map-*[version]*.zip**.
2. Unzip polished-map-*[version]*.zip. This will create the **polished-map-*[version]*** folder.
3. Navigate to the polished-map-*[version]* folder in Explorer.
4. Download fltk-1.3.6-source.tar.bz2 or fltk-1.3.6-source.tar.gz from [**fltk.org**](https://www.fltk.org/software.php) to a new **polished-map-*[version]*\lib** subfolder.
5. Extract fltk-1.3.6-source.tar (you may need a program such as [7-Zip](https://www.7-zip.org/)). This will create the lib\**fltk-1.3.6** folder.
6. Open lib\fltk-1.3.6\abi-version.ide in a text editor such as Notepad and replace "`#undef FL_ABI_VERSION`" with "`#define FL_ABI_VERSION 10306`". Save it.
7. Open lib\fltk-1.3.6\ide\VisualC2010\fltk.sln in Visual Studio 2019. (Other versions may or may not work, I haven't tried.)
8. A "Retarget Projects" dialog will open, since fltk.sln was made for Visual Studio 2010. Click OK to upgrade the Windows SDK version and platform toolset.
9. Go to **Build → Batch Build…**, check the projects **fltk**, **fltkimages**, **fltkpng**, **fltkjpeg**, and **fltkzlib** in the Release configuration, and click the **Build** button.
10. Move all the .lib files from lib\fltk-1.3.6\lib\\\*.lib up to lib\\\*.lib.
11. Copy the lib\FL folder up to include\FL.
12. Open ide\polished-map.sln in Visual Studio 2019.
13. If the Solution Configuration dropdown on the toolbar says Debug, set it to **Release**.
14. Go to **Build → Build Solution** or press F7 to build the project. This will create bin\Release\**polishedmap-plusplus.exe**.
15. Hold down Shift, right-click **install.bat**, and click "**Run as administrator**". This will install Polished Map++ to your account and create a shortcut on your Desktop.


## Linux

### Install dependencies

You need at least g++ 7 for C++11 `<regex>` and C++17 `<string_view>` support.

#### Ubuntu/Debian

Run the following commands:

```bash
sudo apt install make g++ git autoconf
sudo apt install zlib1g-dev libpng-dev libxpm-dev libx11-dev libxft-dev libxinerama-dev libfontconfig1-dev x11proto-xext-dev libxrender-dev libxfixes-dev
```

#### Fedora

Run the following commands:

```bash
sudo dnf install make g++ git autoconf
sudo dnf install zlib-devel libpng-devel libXpm-devel libX11-devel libXft-devel libXinerama-devel fontconfig-devel libXext-devel libXrender-devel libXfixes-devel
```

### Install and build Polished Map++

Run the following commands:

```bash
# Clone Polished Map++
git clone --branch plusplus https://github.com/Rangi42/polished-map.git
cd polished-map

# Build FLTK 1.3.6 with the latest ABI enabled
# (even if you already have libfltk1.3-dev installed)
git clone --branch release-1.3.6 --depth 1 https://github.com/fltk/fltk.git
pushd fltk
./autogen.sh --prefix="$PWD/.." --with-abiversion=10306
make
make install
popd

# Build Polished Map++
# ("export PATH" is needed if bin/fltk-config is not already in your PATH)
export PATH="$PWD/bin:$PATH"
make

# Install Polished Map++
# (tested on Ubuntu and Ubuntu derivatives only; it just copies
#  bin/polishedmap-plusplus and res/app.xpm to system directories)
sudo make install
```

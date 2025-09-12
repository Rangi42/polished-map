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
4. Download fltk-1.4.4-source.tar.bz2 or fltk-1.4.4-source.tar.gz from [**fltk.org**](https://www.fltk.org/software.php) to a new **polished-map-*[version]*\lib** subfolder.
5. Extract fltk-1.4.4-source.tar (you may need a program such as [7-Zip](https://www.7-zip.org/)). This will create the lib\\**fltk-1.4.4** folder.
6. Open Visual Studio, select **Open a local folder**, and open the lib\fltk-1.4.4 folder. This will automatically generate the CMake project with a configuration named **x64-Debug** by default.
7. From the Configuration dropdown, select **Manage Configurations...**, click the green plus sign to add a new configuration, and select **x86-Release** from the list. Set the **Configuration type** to **Release**, set the **Toolset** to **msvc_x86_x64**, and uncheck the **FLTK_GRAPHICS_GDIPLUS** option in the list of CMake variables.
8. Set the active Configuration to **x86-Release**.
9. In the **Solution Explorer**, switch to the **CMake Targets View**, right-click on **fltk_images**, and select **Build fltk_images**. This will also build the other required libraries: fltk, fltk_jpeg, fltk_png, and fltk_z.
10. Move all the .lib files from lib\fltk-1.4.4\out\build\x86-Release\lib\\\*.lib up to lib\\\*.lib.
11. Copy the lib\fltk-1.4.4\\**FL** folder to a new include\\**FL** folder. Also copy lib\fltk-1.4.4\out\build\x86-Release\FL\fl_config.h into include\FL.
12. Open ide\polished-map.sln in Visual Studio 2019.
13. If the Solution Configuration dropdown on the toolbar says Debug, set it to **Release**.
14. Go to **Build → Build Solution** or press F7 to build the project. This will create bin\Release\\**polishedmap-plusplus.exe**.
15. Hold down Shift, right-click **install.bat**, and click "**Run as administrator**". This will install Polished Map++ to your account and create a shortcut on your Desktop.


## Linux

### Install dependencies

You need at least g++ 7 for C++11 `<regex>` and C++17 `<string_view>` support.

CMake (version 3.15 or later) is required for building FLTK 1.4.

#### Ubuntu/Debian

Run the following commands:

```bash
sudo apt install make g++ git autoconf
sudo apt install zlib1g-dev libpng-dev libxpm-dev libx11-dev libxft-dev libxinerama-dev libfontconfig1-dev x11proto-xext-dev libxrender-dev libxfixes-dev libcairo2-dev libpango1.0-dev
```

#### Fedora

Run the following commands:

```bash
sudo dnf install make g++ git autoconf
sudo dnf install zlib-devel libpng-devel libXpm-devel libX11-devel libXft-devel libXinerama-devel fontconfig-devel libXext-devel libXrender-devel libXfixes-devel libcairo2-devel libpango1.0-devel
```

### Install and build Polished Map++

Run the following commands:

```bash
# Clone Polished Map++
git clone --branch plusplus https://github.com/Rangi42/polished-map.git
cd polished-map

# Build FLTK 1.4.4
git clone --branch release-1.4.4 --depth 1 https://github.com/fltk/fltk.git lib/fltk
pushd lib/fltk
cmake -D CMAKE_INSTALL_PREFIX="$(realpath "$PWD/../..")" -D CMAKE_BUILD_TYPE=Release -D FLTK_GRAPHICS_CAIRO=1 -D FLTK_BACKEND_WAYLAND=0 -D FLTK_USE_SYSTEM_LIBPNG=0 -D FLTK_USE_SYSTEM_ZLIB=0
make
make install
popd

# Build Polished Map++
make

# Install Polished Map++
# (tested on Ubuntu and Ubuntu derivatives only; it just copies
#  bin/polishedmap-plusplus and res/app.xpm to system directories)
sudo make install
```

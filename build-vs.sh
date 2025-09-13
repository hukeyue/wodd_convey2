#!/bin/bash
set -e
set -x

function xcopy_with_tag {
tag=$1
cp -fv Release/access.exe ../access-$tag.exe
cp -fv Release/knr_access.exe ../knr_access-$tag.exe
cp -fv Release/odd_convey.exe ../odd_convey-$tag.exe
}

# FIXME
# build with xp
# rm -rf build-xp
# mkdir build-xp
# cd build-xp
# cmake -A Win32 -Tv141_xp ..
# cmake --build . --config Release
# xcopy_with_tag xp
# cd ..

# build with x86
rm -rf build-x86
mkdir build-x86
cd build-x86
cmake -A Win32 ..
cmake --build . --config Release
xcopy_with_tag x86
cd ..

# build with x64
rm -rf build-x64
mkdir build-x64
cd build-x64
cmake -A x64 ..
cmake --build . --config Release
xcopy_with_tag x64
cd ..

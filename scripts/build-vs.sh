#!/bin/bash
set -x
set -e
PWD=$(dirname "${BASH_SOURCE[0]}")
CONFIG=Release
cd $PWD/..

function xcopy_with_tag {
tag=$1
cp -fv ${CONFIG}/access.exe ../access-$tag.exe
cp -fv ${CONFIG}/knr_access.exe ../knr_access-$tag.exe
cp -fv ${CONFIG}/odd_convey.exe ../odd_convey-$tag.exe
}

# FIXME
# build with xp
# rm -rf build-xp
# mkdir build-xp
# cd build-xp
# cmake -A Win32 -Tv141_xp -DCMAKE_CONFIGURATION_TYPES="Debug;MinSizeWithRel;RelWithDebInfo;Release" ..
# cmake --build . --config ${CONFIG}
# xcopy_with_tag xp
# cd ..

# build with x86
rm -rf build-x86
mkdir build-x86
cd build-x86
cmake -A Win32 -DCMAKE_CONFIGURATION_TYPES="Debug;MinSizeWithRel;RelWithDebInfo;Release" ..
cmake --build . --config ${CONFIG}
xcopy_with_tag x86
cd ..

# build with x64
rm -rf build-x64
mkdir build-x64
cd build-x64
cmake -A x64 -DCMAKE_CONFIGURATION_TYPES="Debug;MinSizeWithRel;RelWithDebInfo;Release" ..
cmake --build . --config ${CONFIG}
xcopy_with_tag x64
cd ..

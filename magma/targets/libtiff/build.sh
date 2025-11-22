#!/bin/bash
set -e

##
# Pre-requirements:
# - env TARGET: path to target work dir
# - env OUT: path to directory where artifacts are stored
# - env CC, CXX, FLAGS, LIBS, etc...
##

if [ ! -d "$TARGET/repo" ]; then
    echo "fetch.sh must be executed first."
    exit 1
fi

WORK="$TARGET/work"
rm -rf "$WORK"
mkdir -p "$WORK"
mkdir -p "$WORK/lib" "$WORK/include"

cd "$TARGET/repo"

mkdir -p config
# Manually download these files from a mirror and replace the part of autogen.sh that downloads them
echo "attempting to download from gnu.org"
wget --tries=3 --timeout=15 -O "$(pwd)/config/config.guess" \
    https://raw.githubusercontent.com/pabs3/gnuconfig/refs/heads/master/config.guess || exit 1
wget --tries=3 --timeout=15 -O "$(pwd)/config/config.sub" \
    https://raw.githubusercontent.com/pabs3/gnuconfig/refs/heads/master/config.sub || exit 1


chmod a+x config/config.*
sed -i '/^for file in config\.guess config\.sub/,/^done$/d' autogen.sh
cat ./autogen.sh

./autogen.sh
./configure --disable-shared --prefix="$WORK"
make -j$(nproc) clean
make -j$(nproc)
make install

cp "$WORK/bin/tiffcp" "$OUT/"
$CXX $CXXFLAGS -std=c++11 -I$WORK/include \
    contrib/oss-fuzz/tiff_read_rgba_fuzzer.cc -o $OUT/tiff_read_rgba_fuzzer \
    $WORK/lib/libtiffxx.a $WORK/lib/libtiff.a -lzstd -lz -ljpeg -Wl,-Bstatic -llzma -Wl,-Bdynamic \
    $LDFLAGS $LIBS

#!/bin/bash
set -e

##
# Pre-requirements:
# - env FUZZER: path to fuzzer work dir
##

if [ ! -d "$FUZZER/repo" ]; then
    echo "fetch.sh must be executed first."
    exit 1
fi

cd "$FUZZER/repo"
export CC=clang
export CXX=clang++
export AFL_NO_X86=1
export PYTHON_INCLUDE=/

# Begin arguments
export NO_UNICORN=1
export NO_QEMU=1
export NO_FRIDA=1
export NO_NYX=1
# End arguments

python3 -m venv .venv
export PATH="$FUZZER/repo/.venv/bin:$PATH"

cp GNUmakefile GNUmakefile.bak
sed -i 's/^	-/	/g' GNUmakefile

make clean
make distrib -j"$(nproc)"

# Only install when TEST_BUILD is empty (same logic as Docker)
if [[ -z "${TEST_BUILD:-}" ]]; then
    make install
else
    echo "TEST_BUILD set → skipping make install"
fi

# Restore original file
mv GNUmakefile.bak GNUmakefile

# -------------------------------
# Build the aflpp_driver
# -------------------------------
make -C utils/aflpp_driver -j"$(nproc)"

# -------------------------------
# Create output dirs (Docker did this externally)
# -------------------------------
mkdir -p "$OUT/afl" "$OUT/cmplog"

echo
echo "✔ AFL++ build completed successfully."
echo "✔ Output directories prepared under: $OUT"

## OLD

# make -j$(nproc) || exit 1
# make -C utils/aflpp_driver || exit 1

# mkdir -p "$OUT/afl" "$OUT/cmplog"

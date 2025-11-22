#!/bin/bash
set -e

LLVM_VERSION=19
GCC_VERSION=11

echo "Using LLVM_VERSION=${LLVM_VERSION}"
echo "Using GCC_VERSION=${GCC_VERSION}"

# -------------------------------
# Initial system update
# -------------------------------
apt-get update
apt-get full-upgrade -y
apt-get install -y --no-install-recommends \
    wget ca-certificates apt-utils

rm -rf /var/lib/apt/lists/*


# -------------------------------
# Install build tools and toolchains
# -------------------------------
apt-get update

apt-get -y install --no-install-recommends \
    make cmake automake meson ninja-build bison flex \
    git xz-utils bzip2 wget jupp nano bash-completion less vim joe ssh psmisc \
    python3 python3-dev python3-pip python-is-python3 python3-venv \
    libtool libtool-bin libglib2.0-dev \
    apt-transport-https gnupg dialog \
    gnuplot-nox libpixman-1-dev bc \
    gcc-${GCC_VERSION} g++-${GCC_VERSION} gcc-${GCC_VERSION}-plugin-dev gdb lcov \
    clang-${LLVM_VERSION} clang-tools-${LLVM_VERSION} libc++1-${LLVM_VERSION} \
    libc++-${LLVM_VERSION}-dev libc++abi1-${LLVM_VERSION} libc++abi-${LLVM_VERSION}-dev \
    libclang1-${LLVM_VERSION} libclang-${LLVM_VERSION}-dev \
    libclang-common-${LLVM_VERSION}-dev libclang-rt-${LLVM_VERSION}-dev libclang-cpp${LLVM_VERSION} \
    libclang-cpp${LLVM_VERSION}-dev liblld-${LLVM_VERSION} \
    liblld-${LLVM_VERSION}-dev liblldb-${LLVM_VERSION} liblldb-${LLVM_VERSION}-dev \
    libllvm${LLVM_VERSION} libomp-${LLVM_VERSION}-dev libomp5-${LLVM_VERSION} \
    lld-${LLVM_VERSION} lldb-${LLVM_VERSION} llvm-${LLVM_VERSION} \
    llvm-${LLVM_VERSION}-dev llvm-${LLVM_VERSION}-runtime llvm-${LLVM_VERSION}-tools \
    $([ "$(dpkg --print-architecture)" = "amd64" ] && echo gcc-${GCC_VERSION}-multilib gcc-multilib) \
    $([ "$(dpkg --print-architecture)" = "arm64" ] && echo libcapstone-dev)

rm -rf /var/lib/apt/lists/*

# apt-get update && \
#     apt-get install -y make build-essential clang-15 git wget \
#       python3-dev zlib1g-dev gcc-11-plugin-dev \
#       lld-15 liblld-15 liblld-15-dev liblldb-15 liblldb-15-dev libllvm-15 libomp-15 llvm-15-dev llvm-15-runtime llvm-15-tools

# -------------------------------
# Configure update-alternatives
# -------------------------------
update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-${GCC_VERSION} 0
update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-${GCC_VERSION} 0
update-alternatives --install /usr/bin/c++ c++ /usr/bin/g++-${GCC_VERSION} 0

update-alternatives --install /usr/bin/clang clang /usr/bin/clang-${LLVM_VERSION} 0
update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-${LLVM_VERSION} 0

wget -qO- https://sh.rustup.rs | CARGO_HOME=/etc/cargo sh -s -- -y -q --no-modify-path
PATH=$PATH:/etc/cargo/bin
apt clean -y


export LLVM_CONFIG=llvm-config-${LLVM_VERSION}
export AFL_SKIP_CPUFREQ=1
export AFL_TRY_AFFINITY=1
export AFL_I_DONT_CARE_ABOUT_MISSING_CRASHES=1

git clone --depth=1 https://github.com/vanhauser-thc/afl-cov && \
    (cd afl-cov && make install) && rm -rf afl-cov

# apt-get update && \
#     apt-get install -y make clang-9 llvm-9-dev libc++-9-dev libc++abi-9-dev \
#         build-essential git wget gcc-7-plugin-dev

# update-alternatives \
#   --install /usr/lib/llvm              llvm             /usr/lib/llvm-9  20 \
#   --slave   /usr/bin/llvm-config       llvm-config      /usr/bin/llvm-config-9  \
#     --slave   /usr/bin/llvm-ar           llvm-ar          /usr/bin/llvm-ar-9 \
#     --slave   /usr/bin/llvm-as           llvm-as          /usr/bin/llvm-as-9 \
#     --slave   /usr/bin/llvm-bcanalyzer   llvm-bcanalyzer  /usr/bin/llvm-bcanalyzer-9 \
#     --slave   /usr/bin/llvm-c-test       llvm-c-test      /usr/bin/llvm-c-test-9 \
#     --slave   /usr/bin/llvm-cov          llvm-cov         /usr/bin/llvm-cov-9 \
#     --slave   /usr/bin/llvm-diff         llvm-diff        /usr/bin/llvm-diff-9 \
#     --slave   /usr/bin/llvm-dis          llvm-dis         /usr/bin/llvm-dis-9 \
#     --slave   /usr/bin/llvm-dwarfdump    llvm-dwarfdump   /usr/bin/llvm-dwarfdump-9 \
#     --slave   /usr/bin/llvm-extract      llvm-extract     /usr/bin/llvm-extract-9 \
#     --slave   /usr/bin/llvm-link         llvm-link        /usr/bin/llvm-link-9 \
#     --slave   /usr/bin/llvm-mc           llvm-mc          /usr/bin/llvm-mc-9 \
#     --slave   /usr/bin/llvm-nm           llvm-nm          /usr/bin/llvm-nm-9 \
#     --slave   /usr/bin/llvm-objdump      llvm-objdump     /usr/bin/llvm-objdump-9 \
#     --slave   /usr/bin/llvm-ranlib       llvm-ranlib      /usr/bin/llvm-ranlib-9 \
#     --slave   /usr/bin/llvm-readobj      llvm-readobj     /usr/bin/llvm-readobj-9 \
#     --slave   /usr/bin/llvm-rtdyld       llvm-rtdyld      /usr/bin/llvm-rtdyld-9 \
#     --slave   /usr/bin/llvm-size         llvm-size        /usr/bin/llvm-size-9 \
#     --slave   /usr/bin/llvm-stress       llvm-stress      /usr/bin/llvm-stress-9 \
#     --slave   /usr/bin/llvm-symbolizer   llvm-symbolizer  /usr/bin/llvm-symbolizer-9 \
#     --slave   /usr/bin/llvm-tblgen       llvm-tblgen      /usr/bin/llvm-tblgen-9

# update-alternatives \
#   --install /usr/bin/clang                 clang                  /usr/bin/clang-9     20 \
#   --slave   /usr/bin/clang++               clang++                /usr/bin/clang++-9 \
#   --slave   /usr/bin/clang-cpp             clang-cpp              /usr/bin/clang-cpp-9

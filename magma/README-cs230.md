We have cloned the magma project and modified it in the following ways:
1. Setting the `aflplusplus` fuzzer will copy over code from ./AFLplusplus and build that instead of fetching it from the AFL++ project repository (https://github.com/AFLplusplus/AFLplusplus)
    - We have modified the setup scripts (notably preinstall.sh, fetch.sh, build.sh) in `/fuzzers` to faciliate building the latest version of aflplusplus.

2. We have updated the base Ubuntu image from 18.04 to 24.04

## How to Run

You must be on a unix-based system with Docker running.

1. Edit `./magma/tools/captain/captainrc` to your desired. Here, you define your (1) fuzzer(s), (2) the fuzz targets (aka. the projects to fuzz) and (3) the target programs (aka. the sub-programs/functions in each target). Note the variable you set in each must be prefixed correctly, as variables may be *unique* to the fuzzer and target. 

    In other words, `afl_libtiff_PROGRAMS` will only work for the `afl` fuzzer (not AFL++) on the `libtiff` program. You can also define the length of the campaign.

2. `cd` into `tools/captain/` and run `run.sh`. This will build Docker containers for every combination of (fuzzer, target, program) and then run it.


If you want to run pre-built containers without re-building every container, you can prefix `start.sh` with all necessary environment variables. For example: `FUZZER=aflplusplus TARGET=... PROGRAM=... (...) start.sh`
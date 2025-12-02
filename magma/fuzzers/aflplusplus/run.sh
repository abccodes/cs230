#!/bin/bash

##
# Pre-requirements:
# - env FUZZER: path to fuzzer work dir
# - env TARGET: path to target work dir
# - env OUT: path to directory where artifacts are stored
# - env SHARED: path to directory shared with host (to store results)
# - env PROGRAM: name of program to run (should be found in $OUT)
# - env ARGS: extra arguments to pass to the program
# - env FUZZARGS: extra arguments to pass to the fuzzer
##

set -euo pipefail

if [[ -z "${FUZZ_PCT+x}" ]]; then
  echo "Error: FUZZ_PCT environment variable is not set."
  exit 1
fi

if ! [[ "$FUZZ_PCT" =~ ^[0-9]+$ ]]; then
  echo "Error: FUZZ_PCT must be an integer."
  exit 1
fi

if (( FUZZ_PCT < 0 || FUZZ_PCT > 100 )); then
  echo "Error: FUZZ_PCT must be between 0 and 100 (inclusive)."
  exit 1
fi

echo "FUZZ_PCT is valid: $FUZZ_PCT"

mkdir -p "$SHARED/findings"

flag_cmplog=(-m none -c "$OUT/cmplog/$PROGRAM")

export AFL_SKIP_CPUFREQ=1
export AFL_NO_AFFINITY=1
export AFL_NO_UI=1
export AFL_MAP_SIZE=256000
export AFL_DRIVER_DONT_DEFER=1

"$FUZZER/repo/afl-fuzz" -i "$TARGET/corpus/$PROGRAM" -o "$SHARED/findings" \
    "${flag_cmplog[@]}" -d \
    -F $FUZZ_PCT \
    $FUZZARGS -- "$OUT/afl/$PROGRAM" $ARGS 2>&1

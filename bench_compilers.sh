#!/usr/local/bin/bash
export CLUHOME=$(pwd)
HELLO="hello.clu"

echo "Comparing Binary Sizes:"
ls -lh code/cmp/pclu exe/pclu

echo -e "\n--- Benchmarking GCC-built PCLU ---"
time (for i in {1..20}; do ./code/cmp/pclu -opt -spec lib/*.spc -co $HELLO > /dev/null; done)

echo -e "\n--- Benchmarking PCLU-built PCLU (Self-built) ---"
time (for i in {1..20}; do ./exe/pclu -opt -spec lib/*.spc -co $HELLO > /dev/null; done)

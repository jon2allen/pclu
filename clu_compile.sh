#!/bin/bash

# Simple CLU compilation script
# Usage: ./clu_compile.sh [-o output_name] <filename.clu>

OUTPUT_NAME=""
CLU_FILE=""

while getopts "o:" opt; do
  case $opt in
    o) OUTPUT_NAME=$OPTARG ;;
    *) echo "Usage: $0 [-o output_name] <filename.clu>"; exit 1 ;;
  esac
done

shift $((OPTIND-1))
CLU_FILE=$1

if [ -z "$CLU_FILE" ]; then
    echo "Usage: $0 [-o output_name] <filename.clu>"
    exit 1
fi

BASE_NAME=$(basename "$CLU_FILE" .clu)
if [ -z "$OUTPUT_NAME" ]; then
    OUTPUT_NAME="$BASE_NAME"
fi

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

# Set CLUHOME to the directory where the script is located
export CLUHOME="$SCRIPT_DIR"

if [ ! -f "$CLU_FILE" ]; then
    echo "Error: File '$CLU_FILE' not found."
    exit 1
fi

echo "--- Step 1: Compiling CLU to C and Object ---"
# We ignore the exit code 255 because warnings in stream.spc cause it
"$SCRIPT_DIR/exe/pclu" -opt -spec "$SCRIPT_DIR/lib"/*.spc -co "$CLU_FILE"

# The compiler should produce $BASE_NAME.o
if [ ! -f "$BASE_NAME.o" ]; then
    echo "--- Build failed: No output object file produced ---"
    exit 1
fi

# Link the object file
echo "--- Step 2: Linking $BASE_NAME.o to $OUTPUT_NAME ---"
"$SCRIPT_DIR/exe/plink" -opt -o "$OUTPUT_NAME" "$BASE_NAME.o"

if [ $? -eq 0 ]; then
    echo "--- Build successful! ---"
    echo "You can run it with: ./$OUTPUT_NAME"
else
    echo "--- Linking failed ---"
    exit 1
fi

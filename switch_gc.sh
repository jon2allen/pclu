#!/bin/bash

# script to switch between GC versions
# Usage: ./switch_gc.sh [version]
# e.g., ./switch_gc.sh 8.2.12
#   or  ./switch_gc.sh 7.2

VERSION=$1

if [ -z "$VERSION" ]; then
    echo "Usage: $0 [version]"
    echo "Available versions in code/:"
    ls -d code/gc-* | sed 's/code\/gc-//'
    exit 1
fi

GC_DIR="gc-$VERSION"

if [ ! -d "code/$GC_DIR" ]; then
    echo "Error: version $VERSION not found in code/"
    exit 1
fi

echo "Switching to GC version $VERSION..."

# Update gc symlink
cd code
rm -f gc
ln -s "$GC_DIR" gc

# Update libgc.a symlink
# Boehm GC usually places its libraries in .libs/ after building with libtool
rm -f libgc.a
if [ -f "$GC_DIR/.libs/libgc.a" ]; then
    ln -s "$GC_DIR/.libs/libgc.a" libgc.a
    echo "Linked libgc.a to $GC_DIR/.libs/libgc.a"
elif [ -f "$GC_DIR/libgc.a" ]; then
    # Some builds might put it in the root
    ln -s "$GC_DIR/libgc.a" libgc.a
    echo "Linked libgc.a to $GC_DIR/libgc.a"
else
    echo "Warning: libgc.a not found in $GC_DIR or $GC_DIR/.libs/"
    echo "Make sure you have built the GC version."
fi

# Update gcconfig.h symlink in include directories
# These were sometimes static files, which causes issues with newer GC versions
rm -f include/gcconfig.h
ln -s ../gc/include/private/gcconfig.h include/gcconfig.h
echo "Updated code/include/gcconfig.h symlink"

rm -f include/debug/gcconfig.h
ln -s ../gcconfig.h include/debug/gcconfig.h
echo "Updated code/include/debug/gcconfig.h symlink"

cd ..

echo "GC switched to $VERSION. You may now need to run 'make libpclu' or 'make' to relink the compiler."

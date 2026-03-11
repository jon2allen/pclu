# PCLU FreeBSD 15 (32-bit) Build Instructions

These instructions outline the process for configuring and compiling the Portable CLU (PCLU) system on FreeBSD 15 using a 32-bit environment.

## Prerequisites

*   `gmake` (GNU Make) must be installed.
*   A 32-bit development environment must be configured.

## 1. Prepare the Repository

Navigate to your local clone of the PCLU repository.

```bash
cd pclu
```

## 2. Configure and Build the Garbage Collector

The garbage collector must be built manually before the rest of the system to ensure the static library (`libgc.a`) is created correctly for the FreeBSD host.

```bash
cd code/gc
./configure --host=i386-portbld-freebsd15.0 \
            CFLAGS="-m32 -fcommon -std=gnu89 -Wno-error=int-conversion -Wno-int-conversion -Wno-incompatible-pointer-types -Wno-implicit-function-declaration" \
            LDFLAGS="-m32" \
            --enable-static \
            --disable-shared
gmake
```

## 3. Build the PCLU System

Return to the repository root and use `gmake` to build the rest of the system.

```bash
cd ../..
gmake
```

## 4. Verify the Build (Optional)

You can verify the build by compiling and running the provided `hello.clu` example.

```bash
./clu_compile.sh -o myhello hello.clu
file ./myhello
./myhello
```

### Successful Output:

```
--- Step 1: Compiling CLU to C and Object ---
...
--- Step 2: Linking hello.o to myhello ---
--- Build successful! ---
You can run it with: ./myhello
./myhello: ELF 32-bit LSB executable, Intel i386, version 1 (FreeBSD), dynamically linked, interpreter /libexec/ld-elf.so.1, for FreeBSD 15.0 (1500068), FreeBSD-style, with debug_info, not stripped
Hello, World!
Printing 1 to 20:
1
2
...
20
```

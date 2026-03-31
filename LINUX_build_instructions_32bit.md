# PCLU LINUX (32-bit) Build Instructions

These instructions outline the process for configuring and compiling the Portable CLU (PCLU) system on LINUX using a 32-bit environment.

## Prerequisites

*   `gmake` (GNU Make) must be installed.
*   A 32-bit development environment must be configured.

## 1. Prepare the Repository

Navigate to your local clone of the PCLU repository.

```bash
cd pclu
```

## 2. Configure and Build the Garbage Collector

The garbage collector must be built manually before the rest of the system to ensure the static library (`libgc.a`) is created correctly for the 32bit operation.

```bash
cd code/gc
./configure \
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
myhello: ELF 32-bit LSB pie executable, Intel 80386, version 1 (SYSV), dynamically linked, interpreter /lib/ld-linux.so.2, BuildID[sha1]=d65b6d94688a875607f0fff68e0939b78b762d43, for GNU/Linux 3.2.0, not stripped
Hello, World!
Printing 1 to 20:
1
2
...
20
```

## 5. Building PCLU With Itself (Self-Building)

Once the initial PCLU system is built, you can use it to re-build the compiler itself. This process ensures the compiler is capable of compiling its own source code.

### Step 5.1: Set Environment
Ensure `CLUHOME` is set to the repository root.

```bash
export CLUHOME=$(pwd)
```

### Step 5.2: Generate the Compiler Library
The compiler requires a library file (`cmp.lib`) containing definitions of its internal modules. For a fresh build, this must be generated first.

```bash
cd cmpclu
gmake lib
```

### Step 5.3: Rebuild the Compiler
Compile the CLU source files into C, then compile and link them into a new `pclu` binary.

```bash
gmake
cd ..
gmake compiler
```

### Recording the Build
You can use the `script` command to record the entire self-build process:

```bash
script clu_test1.txt bash -c "export CLUHOME=$(pwd) && cd cmpclu && gmake lib && gmake && cd .. && gmake compiler"
```

The resulting `clu_test1.txt` will contain a full log of the commands and their output.

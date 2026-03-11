# Instructions: Building PCLU with PCLU (Self-Hosting)

These instructions guide you through using the PCLU compiler to rebuild itself from its own CLU source files. This is also known as "bootstrapping" or "self-hosting."

## Prerequisites

1.  **Initial PCLU Build**: You must have already successfully built the initial version of PCLU using the shipped C source (e.g., by running `gmake` in the root).
2.  **Environment Variable**: `CLUHOME` must be set to the absolute path of the PCLU directory.
    ```bash
    export CLUHOME=$(pwd)
    ```
3.  **GMAKE**: You must use `gmake` (GNU Make) on FreeBSD.

---

## Step 1: Create the Compiler Library (`cmp.lib`)

The PCLU compiler requires a special library file called `cmp.lib` which contains the interface specifications for all its internal modules. This must be generated before the source files can be compiled.

```bash
cd cmpclu
gmake lib
```

**What this does**: It runs the existing `pclu` compiler with the `#newlib` and `#dump` commands to merge all `.spc` and `.clu` specifications into a single `cmp.lib` file.

## Step 2: Regenerate the C Source Code

Now, use the existing compiler to translate the CLU source files in `cmpclu/` into intermediate C files.

```bash
gmake
```

**What this does**: It invokes `pclu` on all `.clu` files in the directory. The output C files are placed in `../code/cmp/`.

## Step 3: Link the New Compiler Binary

Once the C files are regenerated, they need to be compiled by GCC and linked with the PCLU runtime and Garbage Collector.

```bash
cd ..
gmake compiler
```

**What this does**:
1.  Compiles the newly generated `.c` files in `code/cmp/` into `.o` objects.
2.  Links them into a new executable at `exe/pclu`.

---

## Recording the Process

To create a record of all commands executed (as requested for `clu_test1.txt`), you can run the following one-liner from the repository root:

```bash
script clu_test1.txt bash -c "export CLUHOME=$(pwd) && cd cmpclu && gmake lib && gmake && cd .. && gmake compiler"
```

## Verification

After building, verify that the new compiler is functional by compiling a simple test:

```bash
./exe/pclu -opt -spec lib/*.spc -co hello.clu
./exe/plink -opt -o hello_test hello.o
./hello_test
```

---
**Note**: If you encounter "type mismatch" or "bad file format" errors during Step 1 or 2, ensure that your `CLUHOME` is correct and that you have not accidentally mixed 32-bit and 64-bit object files.

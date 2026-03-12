# 64-bit Porting Status (LP64)

This document tracks the progress of porting PCLU from it's legacy 32-bit (ILP32) origins to a modern 64-bit (LP64) architecture.

## 1. Current Progress Summary
- **Target Architecture**: x86-64 (AMD64)
- **Status**: Operational (Beta)
- **Main Compiler**: `pclu` successfully built as native 64-bit and verified to emit 64-bit capable C code and binary library formats.
- **Linker**: `plink` updated to link 64-bit objects natively.
- **Runtime**: Native 64-bit linkage against `gc-7.2`.

## 2. Infrastructure Changes
### Build System
- Removed `-m32` flags and 32-bit library dependencies.
- Updated `plink` to use native `gcc` 64-bit linkage.
- `pclu` and `plink` now consistently produce 64-bit ELF executables.
- Switched to `gmake` for better cross-platform compatibility (FreeBSD/Linux).

### Garbage Collector (`code/gc-7.2`)
- Re-configured for native 64-bit.
- Linked `code/libgc.a` to the 64-bit `.libs` build artifact.

### Core Headers (`code/include/pclu_sys.h`)
- Updated `intOPmaxint`, `intOPleastint`, and `UNINIT` to 64-bit values.
- Updated `intOPsqrtmax` for 64-bit multiplication checks.
- Verified `CLUREF` union alignment (now 8 bytes).

## 3. Implementation Fixes
| Module | Change | Reason |
| :--- | :--- | :--- |
| `_wordvec.c` | `<< 2` -> `<< 3` | Pointer scaling/indexing from 32-bit units to 64-bit. |
| `gc_read.c` | `+ 4` -> `+ 8` | Binary header and offset scaling for serialized objects. |
| `gc.c` | `/ 4` -> `/ 8` | Heap usage calculation normalization. |
| `hash_obj.c`| `/ 4` -> `/ 8` | Address-based hashing alignment. |
| `gcd.equ` | `4` -> `8`, `8` -> `16` | Adjusted constants `CLUREFSZ` and `GCD_REF_SIZE` for 64-bit serialization. |
| `gcd_tab.clu` | `4` -> `CLUREFSZ` | Replaced hardcoded word size literals in library dumper/merge logic. |
| `gc_read.c` / `gcd_tab.c` | Patched C code | Bootstrapping generated C files to use 8-byte logic for headers/offsets. |
| `_chan.c` | `sizeof(CLUREF)` | Fixed word-size I/O (puti, geti, putw, getw) to handle 8-byte integers/pointers. |

## 4. Verification Tests
- [x] **Compiler Self-Help**: `pclu -help` runs without segfault.
- [x] **Hello World**: `hello.clu` compiles, links, and prints correctly in native 64-bit.
- [x] **64-bit Arithmetic**: Test `2e9 + 2e9` results in `4000000000` (success).
- [x] **Large Constants**: Verified `10000000000` (10 billion) works without truncation in `test_const.clu`.
- [x] **Library Merge/Dump**: `misc.lib` and `lowlev.lib` merging works without "bad file format" error.
- [x] **64-bit Dumper**: Compiler now correctly emits 64-bit aligned binary libraries (`.lib`).
- [ ] **Complex Objects**: Testing CLU arrays and records with 64-bit pointers.
- [ ] **GC Stability**: Need to run full stress test of the garbage collector in 64-bit.

## 5. Known Issues / Remaining Work
- **Hardcoded Masks**: Some modules (e.g., `random.c`) still use `0x7FFFFFFF` masks which may need expansion or adjustment to match 64-bit integer semantics.
- **Bit Manipulation**: `_wordvec` has bit-field logic (`get_byte`/`set_byte`) assumes 32-bit boundaries.

---
*Created: March 10, 2026*
*Updated: March 11, 2026*

## 6. Major Blockers Resolved (March 11, 2026)

### **The `negative_size` \& "Word Size 0" Library Crash**
During the build of `cludent` and `cmp.lib`, the compiler would crash with `failure: negative_size` or report `bad file format` despite headers appearing correct.

**Resolution:**
The runtime channel implementation (`_chan.c`) was found to have multiple hardcoded word-size assumptions:
- `_chanOPputi` and `_chanOPgeti` were using a fixed size of `1` byte for integer I/O. On a 64-bit system, this caused a 7-byte misalignment in the stream for every word read, corrupting subsequent headers.
- `_chanOPputw` was passing a "word count" to the `write` syscall instead of a "byte count" ($words \times 8$), resulting in truncated output files (1/8th of intended size).

**Fixed:**
- Replaced hardcoded `1` and word-based counts in `_chan.c` with `sizeof(CLUREF)` (8 bytes).
- Rebuilt `libpclu_opt.a`, relinked `pclu`, and regenerated all `.lib` files.
- Verified that `cludent` now compiles and runs correctly.

## 7. Status Update (March 12, 2026)

### **Clean Build and Verification**
A full system rebuild was performed starting from `gmake veryclean`.

**Results:**
- **Garbage Collector**: Successfully rebuilt in 64-bit mode. Passed all internal `gctest` consistency checks on the LP64 architecture.
- **Compiler**: `pclu` was successfully rebuilt and verified as a native 64-bit ELF binary.
- **Runtime Execution**: Verified compiler output with a 64-bit "Hello World" (`hello.clu`), which compiled, linked, and executed without issues.
- **Regression**: The `cludent` build encountered a `bad file format` error during the high-level library merging phase (`gmake cludent`). This indicates that while individual object compilation works, the complex logic for merging multiple `.lib` files into a single context still contains 64-bit alignment or offset edge cases that were thought to be resolved on March 11.

**Next Steps:**
- Investigate the `bad file format` regression in `merge` logic specifically for multi-library scenarios.

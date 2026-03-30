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

## 7. Library Merge Issue (March 30, 2026)

**Status: Under investigation**

The library merge (`#me lowlev.lib`) fails with "bad file format". This blocks `cludent` and `useful.lib` builds.

### Root Cause
The dumper (`gcd_tab.c`, `store_id`) writes the header with `pos=3`, meaning grind data starts at `buf[3]`. The grind function patches `buf[1]` and writes 8 words at offset 8. The resulting file layout is:
```
Offset 0:   "DWC1\0\0\0\0"        (8 bytes)
Offset 8:   buf[1] = next addr     (patched)
Offset 16:  buf[2] = grind item 1  (bzero expected, but NOT zeroed)
Offset 24:  buf[3] = GCD_REF (33)  (root type written by grind)
Offset 32:  buf[4] = root addr     (0)
```

The reader (`gc_read.c`) expects GCD_REF at offset 16 (buf[2]) but finds grind data there.

### What Works
- `make veryclean && make gc clu` builds successfully
- Compiler (`pclu`) compiles and links natively as 64-bit
- `misc.lib` dumps successfully
- `lowlev.lib` dumps successfully (without `#forget` clause)
- Hello World compiles, links, and runs correctly
- Library dump creates valid `.lib` files (verified via hex dump)

### What Doesn't Work
- Library merge (`#me lowlev.lib`) fails with "bad file format"
- `cludent` build blocked by merge failure
- `#forget` clause causes `failure: bounds` during dump

### Investigation Notes
- The `_wordvecOPcreate` bzero fix doesn't prevent garbage in `buf[2]`
- Modifying `store_id` to set `pos=5` with explicit header writes causes `failure: bounds`
- The `grind` function patches `buf[1]` but `buf[2]` remains uninitialized
- The `gcb$getb` function uses `wvec$move_w2b` which reads from word-aligned positions
- The `#forget` directive may trigger bounds errors in the dump process

### Needed Fix
Either:
1. Add a padding word (buf[2]=0) in `store_id` and shift grind start to pos=4, OR
2. Fix the reader to match the actual writer format (GCD_REF at data[1] not data[0])

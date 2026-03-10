# 64-bit Porting Status (LP64)

This document tracks the progress of porting PCLU from it's legacy 32-bit (ILP32) origins to a modern 64-bit (LP64) architecture.

## 1. Current Progress Summary
- **Target Architecture**: x86-64 (AMD64)
- **Status**: Operational (Alpha)
- **Main Compiler**: `pclu` successfully built as native 64-bit.
- **Runtime**: Native 64-bit linkage against `gc-7.2`.

## 2. Infrastructure Changes
### Build System
- Removed `-m32` flags and 32-bit library dependencies.
- Updated `plink` to use native `gcc` 64-bit linkage.
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

## 4. Verification Tests
- [x] **Compiler Self-Help**: `pclu -help` runs without segfault.
- [x] **Hello World**: `hello.clu` compiles, links, and prints correctly.
- [x] **64-bit Arithmetic**: Test `2e9 + 2e9` results in `4000000000` (success).
- [ ] **Complex Objects**: Testing CLU arrays and records with 64-bit pointers.
- [ ] **GC Stability**: Need to run full stress test of the garbage collector in 64-bit.

## 5. Known Issues / Remaining Work
- **Hardcoded Masks**: Some modules (e.g., `random.c`) still use `0x7FFFFFFF` masks which may need expansion or adjustment to match 64-bit integer semantics.
- **Bit Manipulation**: `_wordvec` has bit-field logic (`get_byte`/`set_byte`) that assumes 32-bit boundaries.
- **Bootstrap Phase**: The original `pclu` binaries used to bootstrap the system are 32-bit; currently relying on existing 64-bit build artifacts.

---
*Created: March 10, 2026*

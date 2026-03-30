# Build Review - March 30, 2026

## Overview

Reviewed the PCLU build system (gmake) and 64-bit porting status. Performed a full clean rebuild from source.

## Build System (gmake) Notes

- **Top-level `Makefile`** auto-detects platform via `uname -s` and symlinks `Makefile.linux` into each subdirectory
- **Build flow**: `configure` → `gc` → `libpclu` → `compiler` → `clulibs` → `cludent`
- **gmake is required** — GNU-specific features (`:=` shell assignments, `export`) prevent use of BSD `make`
- All 11 `Makefile.linux` files share identical CFLAGS: `-std=gnu89 -fcommon -Wno-error=int-conversion -Wno-int-conversion -Wno-incompatible-pointer-types -Wno-implicit-function-declaration`
- No `-m32` flags remain — build is fully native 64-bit

## Build Results

| Component | Status | Details |
|-----------|--------|---------|
| GC (gc-7.2) | ✅ Builds | Native 64-bit, passes gctest |
| Compiler (`pclu`) | ✅ Builds | 64-bit ELF, links natively |
| Runtime (`libpclu_opt.a`) | ✅ Works | 64-bit archive |
| Hello World | ✅ Runs | Compiles, links, prints correctly |
| Large constants | ✅ Works | `10000000000` (10 billion) prints correctly |
| Library dump (`.lib` creation) | ✅ Works | misc.lib, lowlev.lib, useful.lib created |
| Library merge (`.lib` re-read) | ❌ Fails | "bad file format" on merge |
| cludent build | ❌ Blocked | Depends on library merge |

## Root Cause of Merge Failure

The library **dumper** (`gcd_tab.c`) and **reader** (`gc_read.c`) were hand-patched for 64-bit in commit `af54dc1`, but the March 27 commit (`7749992`) reverted critical I/O fixes in `_chan.c`:

- `_chanOPputi` / `_chanOPgeti`: went from `sizeof(CLUREF)` (8) back to `1` byte
- `_chanOPputw`: byte counting reverted

These reverted I/O functions corrupt the binary `.lib` format during writing, making them unreadable.

## What Was Done

- Restored `af54dc1` versions of `_chan.c`, `string.c`, `_wordvec.c`, `gcd_tab.clu`, `gc_read.clu`, and `pclu_sys.h`
- Updated `MAX_STR` from 150000 to 1000000000 for large object serialization
- Added `bzero` calls to `_wordvecOPcreate` and `_wordvecOPcopy` for memory safety
- Verified library headers via hex dump — "DWC1" magic present at offset 0, format looks correct
- Library merge still fails — needs deeper investigation into `gc_read.c` reader's header parsing logic

## Key Files Modified

- `code/sysasm/Opt/_chan.c` — restored 64-bit I/O from af54dc1
- `code/sysasm/Opt/string.c` — restored from af54dc1
- `code/libasm/Opt/_wordvec.c` — restored from af54dc1 + bzero fix
- `code/libclu/Opt/gc_read.c` — restored from af54dc1
- `code/libclu/Opt/gcd_tab.c` — restored from af54dc1
- `code/libclu/Opt/gc_dump.c` — restored from af54dc1
- `code/include/pclu_sys.h` — restored from af54dc1 + MAX_STR fix
- `lib/clu/gcd_tab.clu` — restored from af54dc1
- `lib/clu/gc_read.clu` — restored from af54dc1

## Remaining 64-bit Issues (from 64BIT_PORT.md)

- **Complex objects**: CLU arrays/records with 64-bit pointers still need testing
- **GC stability**: No full stress test yet
- **Hardcoded masks**: `random.c` still uses `0x7FFFFFFF`
- **Library merge**: Serialization format mismatch between dumper and reader blocks cludent build

---
*Created: March 30, 2026*

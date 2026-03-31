# PCLU 32-to-64-bit Port: Bounds Failure Fix

**Date**: March 30, 2026  
**Issue**: "failure: bounds" when using `#forget` directive during library build  
**Goal**: Get `cludent` building

## Problem Description

When building the PCLU libraries (`lowlev.lib`, `useful.lib`, `misc.lib`), the build would fail with "failure: bounds" when the `#forget` directive was used in the Makefile. This occurred during the `DU$forget_specs` operation.

### Debug Output Analysis

Initial debug output showed:
```
DEBUG forget_specs: d.index = 108, keepmax = 30
DEBUG forget_specs: accessing store[108], ext_low=1, ext_high=172, ext_size=172
failure: bounds
```

All printed store accesses showed indices within bounds (1-172), yet bounds check still failed.

## Root Cause Investigation

### 1. Library Format Mismatch (Bad File Format)

Before fixing the bounds issue, there was a "bad file format" error when reading libraries. This was caused by word-size mismatch between 32-bit libraries and 64-bit compiler:

- **32-bit word**: 4 bytes (CLUREFSZ = 4)
- **64-bit word**: 8 bytes (CLUREFSZ = 8)

The dumper (`gcd_tab.c`) wrote data assuming 4-byte words, but the reader (`gc_read.c`) expected 8-byte words on 64-bit.

### 2. gcd_tab.c store_id Issue

The `store_id` procedure was setting `pos := 3` without writing the GCD_REF header at positions 2-4:

```clu
% Original (broken):
bvec$store(w2b(buf), 1, 'D')
bvec$store(w2b(buf), 2, 'W')
bvec$store(w2b(buf), 3, 'C')
bvec$store(w2b(buf), 4, '1')
pos := 3  % WRONG - should write GCD_REF at buf[2..4]
```

### 3. gcd_tab.c grind Word Count Issue

The `grind` function was writing only 1 word instead of 4 words:

```c
// Broken (writes 1 word = 8 bytes on 64-bit):
err = _chanOPputw(ch, buf, CLU_1, CLU_1, CLU_0);

// Fixed (writes 4 words = 32 bytes):
err = _chanOPputw(ch, buf, CLU_1, CLU_4, CLU_0);
```

## Fixes Applied

### 1. gcd_tab.clu - store_id

**File**: `lib/clu/gcd_tab.clu`

Changed from:
```clu
bvec$store(w2b(buf), 1, 'D')
bvec$store(w2b(buf), 2, 'W')
bvec$store(w2b(buf), 3, 'C')
bvec$store(w2b(buf), 4, '1')
pos := 3
```

To:
```clu
bvec$store(w2b(buf), 1, 'D')
bvec$store(w2b(buf), 2, 'W')
bvec$store(w2b(buf), 3, 'C')
bvec$store(w2b(buf), 4, '1')
buf[2] := 0
buf[3] := GCD_REF
buf[4] := 0
pos := 5
```

### 2. gcd_tab.c - Generated C Code

**File**: `code/libclu/Opt/gcd_tab.c`

**store_id procedure** (lines 2285-2303): Added code to write the GCD_REF header:
```c
LINE(382);
{
    err = _wordvecOPstore(gcd_storeOPbuf, CLU_2, CLU_0);
    if (err != ERR_ok) goto ex_0;
}

LINE(383);
{
    err = _wordvecOPstore(gcd_storeOPbuf, CLU_3, CLU_33);
    if (err != ERR_ok) goto ex_0;
}

LINE(384);
{
    err = _wordvecOPstore(gcd_storeOPbuf, CLU_4, CLU_0);
    if (err != ERR_ok) goto ex_0;
}

LINE(385);
{
    gcd_storeOPpos.num = 5;
}
```

**grind procedure** (line 325): Changed word count from 8 to 4:
```c
// Before:
err = _chanOPputw(ch, buf, CLU_1, CLU_8, CLU_0);

// After:
err = _chanOPputw(ch, buf, CLU_1, CLU_4, CLU_0);
```

**grindc procedure** (line 527): Same fix - changed from 8 to 4.

### 3. Debug Output Added

**File**: `code/cmp/du.c`

Added debug output to track bounds failures:
- Line 263: Bounds check on fixups array
- Line 299: Bounds check on store array (read_specs)
- Line 344: Bounds check on store array
- Line 410: Bounds check on cache array
- Line 452: Bounds check on cache array
- Line 494: Bounds check on store array
- Line 544: Bounds check on store array
- Line 558: Bounds check on store array
- Line 648: Bounds check on store array (forget_specs)
- Line 662: Bounds check on fixups array (forget_specs)
- Line 677: Bounds check on cache array (forget_specs)
- Line 729: Bounds check on store array (keep)
- Line 799: Bounds check on store array (reset)
- Line 809: Bounds check on fixups array (reset)
- Line 825: Bounds check on cache array (reset)

**File**: `code/libclu/Opt/gc_read.c`

Added debug output for library format checking (lines 257, 278).

## Debugging Approach

1. Added `fprintf` debug statements at all bounds check failure points
2. Traced which array access was failing
3. Found that all three arrays (store, fixups, cache) grew in sync
4. Discovered the issue was actually in the library WRITER (gcd_tab.c), not the bounds checking

## Key Insight

The bounds failure wasn't actually a bounds issue - it was caused by corrupted library files. The gcd_tab.dumper was writing data in 32-bit format while the reader expected 64-bit format. When the writer was fixed to write the correct format, the bounds failure disappeared.

## Result

After fixes applied:
- `lowlev.lib` builds successfully (951,616 bytes)
- `useful.lib` builds successfully (545,856 bytes)  
- `misc.lib` builds successfully (257,152 bytes)

## Remaining Issues

The cludent build still has undefined references:
- `_adv`, `_bytevec`, `_chan`, `_cvt`, `_free_space`, `_gc`
- `array_replace`, `file_exists`, `get_argv`, `hash`, `table`

This appears to be a separate issue related to the cluent source files referencing items from the standard library that aren't properly merged or defined. This is unrelated to the bounds failure fix.

## Files Modified

1. `lib/clu/gcd_tab.clu` - CLU source for store_id
2. `code/libclu/Opt/gcd_tab.c` - Generated C code (3 locations)
3. `code/cmp/du.c` - Debug output for bounds checking
4. `code/libclu/Opt/gc_read.c` - Debug output for library format

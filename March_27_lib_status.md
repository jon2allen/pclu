# PCLU 64-bit Library Status - March 27

## Objective
Stabilize the 64-bit PCLU build by resolving "failure: bounds" and "bad file format" errors during library generation (`make libs`) and merging.

## Status Summary
- **_wordvec.c**: Completely rewritten for 64ndndndnd-bit Word/Byte indexing. ALL union accesses now use the `.num` member (or the scalar direct access for longs). Added broad diagnostic logging to identify bounds failures.
- **pclu_sys.h**: `MAX_STR` increased to 1GB to prevent "failure: toobig" during large object serialization.
- **gc_read.clu**: Flexible header scanner implemented to handle "DWC1" signature and locate the Root Reference (GCD_REF) dynamically (found at Word 4 in current builds).
- **gcd_tab.clu**: Alignment for strings adjusted for 64ndndndnd-bit word boundaries.

## Known Challenges
- **Bounds Failure**: `make libs` still signals `failure: bounds` during `lowlev.lib` or `useful.lib` generation. This may be coming from `_wordvec$move_b2w` or a different cluster (e.g., `sequence` or `array`).
- **Header Synchronization**: The Root Reference (value 33) is appearing at Word 4 (offset 24) instead of Word 3. `gc_read` has been adjusted to find it, but the inconsistent writing in `store_id` (specifically that `buf[3] := GCD_REF` ends up at Byte 24ndnd) needs investigation.

## Immediate Next Steps
1. **Rebuild and Log**: Perform a clean bootstrap and Capture `stderr` to identify the specific `_wordvec` index that fails bounds checking.
2. **Audit Array/Sequence**: If `_wordvec` logs are silent, audit `code/sysasm/Opt/array.c` and `sequence.c` for 32ndndndnd-bit word size assumptions (specifically searching for `4` vs `8` or `CLUREFSZ`).
3. **Verify cludent**: Once `libs` are stable, finalize the `cludent` build to confirm the entire toolchain is functional.

# FreeBSD Compilation Fixes Report

This report documents the specific changes made to allow Portable CLU (PCLU) to compile and run on **FreeBSD 15 (32-bit)**. 

## 1. Build System Setup
> [!IMPORTANT]
> On FreeBSD, you **must** use `gmake` (GNU Make). The system's default `make` (BSD Make / bmake) is incompatible with the modernized cross-platform syntax used in this project.

- **Makefile links**: Configured the build system to use `Makefile.freebsd` by linking it to `Makefile.linux` across all 11 subdirectories.
- **Modern Compiler Compatibility**:
  - Added `-std=gnu89` to all `CFLAGS` to support the legacy K&R-style C code generated from CLU.
  - Added warning suppression flags (`-Wno-error=int-conversion`, etc.) to prevent build failures on modern `clang`.
  - Added `-fcommon` to allow merging of global symbols across object files.

## 2. Kernel/Header Compatibility
- **Signal Mask Access**: Fixed errors in `sleep.c`, `_set_alarm.c`, and `_change_signal.c` by accessing the signal mask via `sa_mask.__bits[0]` (FreeBSD specific) instead of `__val[0]` (Linux specific).
- **External Error Variables**: Guarded `extern int sys_nerr` in `util.c` with `#if !defined(FREEBSD)` to avoid conflicts with FreeBSD 15's native headers.
- **Directory Operations**: 
  - Switched from `dp->d_namlen` to `strlen(dp->d_name)` in `_all_dir_entries_.c` to remain compatible with FreeBSD's `struct dirent`.
  - Replaced `close(dirp)` with `closedir(dirp)` for directory handle safety.

## 3. Garbage Collector (GC) Integration
- Successfully built the bundled Boehm GC with 32-bit compatibility.
- Fixed the `libgc.a` symlink in the `code/` directory to point to the freshly built library in `gc-7.2/.libs/libgc.a`.
- Ensured thread support was enabled via FreeBSD-compatible signal handling in the GC.

## 4. Initialization and Casting
- **`oneof.c` Fixing**: Corrected implicit cast and initialization errors in `oneofOPnew` calls by using C99 designated initializers for the `CLUREF` union.
- **Signal Handler Casting**: Added explicit function pointer casts for the `clu_alarm` handler in `_set_alarm.c` to satisfy strict typing.

## 5. Runtime and Self-Building Fixes
Specific runtime issues were addressed to enable the compiler to build itself:

- **Error String Handling**: In `eval1.c` and `eval2.c`, replaced direct pointer-to-int assignments (`err_UNIQ.num = elist[0].num`) with the proper runtime helper `_pclu_erstr(err)`. This ensures error messages are correctly resolved from the string table instead of causing memory corruption.
- **Floating-Point Precision**: Adjusted a real constant in `penv.c` from `1.892883e-308` to `9.999999e-39` to prevent underflow issues on the 32-bit architecture.
- **Symbol Conflict Resolution**: Renamed the internal record symbol `record_line_stmt_ops` to `record_line_stmt_1_ops` in `syntax.c` to avoid linkage collisions between different generated internal modules.
- **Prototype Correctness**: Reorganized and added missing `extern` declarations for iterator bodies (`_IB_` functions) in `ce.c`, `xlib.c`, and `top1.c` to satisfy modern compiler scoping requirements and prevent "implicit declaration" errors during the self-build phase.

---
**Verified**: The compiler (`pclu`), linker (`plink`), and runtime libraries (`.lib`) are fully operational on FreeBSD, and the compiler has successfully completed a **full self-build loop**.

# Unified Cross-Platform Build Report

This report documents the architectural changes made to the PCLU build system to support both **Linux** and **FreeBSD** from a single, unified codebase/branch.

## 1. Automated Platform Detection
Managed to consolidate the multi-platform build process in the top-level `Makefile`:
- **Shell-based Probing**: Replaced hardcoded `PLATFORM` variables with:
  ```makefile
  PLATFORM := $(shell uname -s | tr '[:upper:]' '[:lower:]')
  ```
- **Unified Logic**: The same `Makefile` now automatically configures symlinks for the specific OS it is running on. 
> [!IMPORTANT]
> Developers on FreeBSD or other non-Linux systems **must** use `gmake`. The default BSD `make` (bmake) does not support the shell probing and export syntax used for cross-platform automation.

## 2. Eliminating Hardcoded Native Flags
Previously, the code relied on explicit `-DFREEBSD` or `-DLINUX` flags in Makefiles. This has been modernized:
- **Compiler Introspection**: The codebase now utilizes the compiler's built-in macros (`__FreeBSD__`, `__linux__`, and `__ELF__`).
- **Header-Driven Macros**: The internal header `pclu_sys.h` (which includes `gcconfig.h`) now automatically derives the correct internal `FREEBSD` or `LINUX` definitions based on the OS environment, ensuring consistent behavior without manual Makefile intervention.

## 3. POSIX Standardization
To reduce platform-specific `#ifdef` blocks, several components were updated to follow POSIX standards:
- **Directory Traversal**: Normalized directory handling by using standard POSIX functions like `strlen(dp->d_name)` for entry lengths and `closedir()` for cleanup.
- **Linker Commonality**: Standardized on `-fcommon` and `-m32` across all platforms to ensure consistent memory layout and symbol resolution regardless of the host OS default behavior.

## 4. Single-Branch Maintenance
Changes to global headers like `include/pclu_sys.h` (e.g., renaming the `restrict` keyword to `restr`) were implemented in a way that benefits all platforms. This allows the project to maintain a single set of C-source files that are portable across any 32-bit x86 Unix-like environment.

---
**Summary**: PCLU is now a "clone and build" project for both Linux and FreeBSD.

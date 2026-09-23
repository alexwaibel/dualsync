# DualSync

DualSync is a RomM client for Nintendo DS-family homebrew systems. The initial
target is Nintendo DSi running through TWiLight Menu++; a Nintendo 3DS client is
planned around the same portable core.

The distributed `.nds` remains DS-compatible but is expected to run in DSi mode
for the additional memory, CPU speed, and WPA2 networking. A constrained DS-mode
compatibility tier may be explored later.

The project is in its hardware-prototyping phase. See
[docs/PLAN.md](docs/PLAN.md) for the scope and delivery plan.

## Repository layout

```text
apps/         Platform-specific applications
build/        Generated build and distribution artifacts
containers/   Isolated target build environments
core/         Portable RomM and save-sync logic
docs/         Project planning and design documentation
scripts/      Developer workflow entry points
```

Platform APIs must not be included from `core/`. Each application compiles the
core with its own target toolchain and supplies networking, filesystem, clock,
input, and UI implementations.

## Build

Docker is the only host dependency.

```bash
./scripts/dualsync dsi
./scripts/dualsync test
```

The DSi build is written to `build/dist/dsi/dualsync-dsi.nds`. Intermediate
files used for incremental builds and debugging remain under `build/obj/`.

## Development checks

Formatting follows an LLVM-derived C style with four-space indentation, Allman
braces, and a 100-column limit.

```bash
./scripts/dualsync format
./scripts/dualsync format-check
./scripts/dualsync lint
./scripts/dualsync tidy
./scripts/dualsync test
./scripts/dualsync check
```

These commands run through project containers and do not require host-installed
C development tools. CMake is the sole build definition; Docker Compose selects
the appropriate host or console toolchain. Portable-core unit tests use cmocka
and are registered with CTest.

To enable the repository's pre-commit formatting check for the current clone:

```bash
./scripts/dualsync install-hooks
```

The hook checks formatting without modifying staged files. Git hooks can be
bypassed, so CI runs the complete check independently.

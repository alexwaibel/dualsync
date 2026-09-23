# DualSync

DualSync is a RomM client for Nintendo DS-family homebrew systems. The initial
target is Nintendo DSi running through TWiLight Menu++; a Nintendo 3DS client is
planned around the same portable core.

The project is in its hardware-prototyping phase. See
[docs/PLAN.md](docs/PLAN.md) for the scope and delivery plan.

## Repository layout

```text
apps/         Platform-specific applications
build/        Generated build and distribution artifacts
containers/   Isolated target build environments
core/         Portable RomM and save-sync logic
docs/         Project planning and design documentation
```

Platform APIs must not be included from `core/`. Each application compiles the
core with its own target toolchain and supplies networking, filesystem, clock,
input, and UI implementations.

## Build

Docker is the only host dependency.

```bash
make dsi
make test
```

The DSi build is written to `build/dist/dsi/dualsync-dsi.nds`. Intermediate
files used for incremental builds and debugging remain under `build/obj/`.

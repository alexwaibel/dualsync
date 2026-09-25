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
.devcontainer/ Canonical development and CI environment
apps/         Platform-specific applications
build/        Generated build and distribution artifacts
core/         Portable RomM and save-sync logic
docs/         Project planning and design documentation
scripts/      Developer workflow entry points
```

Platform APIs must not be included from `core/`. Each application compiles the
core with its own target toolchain and supplies networking, filesystem, clock,
input, and UI implementations.

## Build

### Host requirements

The recommended environment uses Visual Studio Code Dev Containers and requires:

- Git
- Docker Engine or Docker Desktop
- Visual Studio Code with the Dev Containers extension
- Internet access when building an environment whose image layers or pinned
  packages are not already cached

Open the repository in Visual Studio Code and select **Dev Containers: Reopen in
Container**. The editor, terminals, CMake integration, host analysis tools, and
BlocksDS toolchain then run in the same versioned environment used by CI.

To run the project commands directly from the host instead, also install Docker
Compose v2, available as `docker compose`, and use a POSIX-compatible shell:

```bash
./scripts/dualsync dsi
./scripts/dualsync test
```

No host C compiler, CMake installation, BlocksDS SDK, GitHub CLI, or development
libraries are required. On Windows, use Docker Desktop with Dev Containers or
run the CLI workflow through WSL.

The DSi build is written to `build/dist/dsi/dualsync-dsi.nds`. Intermediate
files used for incremental builds and debugging remain under `build/obj/`.

The current Phase 0 build probes the public RomM demo heartbeat over verified
HTTPS by default. Override it with a full heartbeat URL to test another server:

```bash
DUALSYNC_PROBE_URL=https://romm.example.com/api/heartbeat ./scripts/dualsync dsi
```

The probe uses the embedded ISRG Root X1 and X2 certificates for Let's Encrypt
servers. It requires DSi mode and a network configured in the console's system
settings.

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

Inside the Dev Container these commands run directly. From the host they
automatically start the same environment through Docker Compose. CMake is the
sole build definition, and portable-core unit tests use cmocka through CTest.

The development image pins its base-image digest and direct package versions.
Builds do not perform implicit package upgrades. To report the active host and
DSi toolchain versions:

```bash
./scripts/dualsync versions
```

The image currently builds on the official BlocksDS `slim` image to reuse its
maintained Wonderful and BlocksDS installation. This is a bootstrap choice, not
a platform boundary. When 3DS development begins, the official devkitPro
toolchain will be installed alongside Wonderful under its separate
`/opt/devkitpro` prefix so the same Dev Container supports both applications.

Dependabot checks the pinned Docker images and GitHub Actions weekly. Package
version changes are reviewed and validated through the normal project checks
instead of being applied during unrelated builds.

The same Dev Container is prebuilt as
`ghcr.io/alexwaibel/dualsync-devcontainer`. CI builds the checked-out
`.devcontainer` definition and runs the complete checks inside it, using the
published `cache` image only as a BuildKit cache. Source files and tests always
come from the commit or pull request being checked.

Pull requests never publish images. When the Dev Container definition changes
on `main`, a separate trusted workflow builds it, runs the complete checks, and
only then publishes commit-specific and moving `cache` tags. The cached image
also preserves successfully built package layers if an upstream package later
becomes unavailable. The GHCR package should be public so local builds and CI
can use it without registry credentials.

To enable the repository's pre-commit formatting check for the current clone:

```bash
./scripts/dualsync install-hooks
```

The hook checks formatting without modifying staged files. Git hooks can be
bypassed, so CI runs the complete check independently.

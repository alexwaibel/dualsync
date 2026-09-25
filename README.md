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

## Development environment

The Dev Container is the canonical development and CI environment. Using it
requires:

- Git
- Docker Engine or Docker Desktop
- Visual Studio Code with the Dev Containers extension
- Internet access when building an environment whose image layers or pinned
  packages are not already cached

Open the repository in Visual Studio Code and select **Dev Containers: Reopen in
Container**. This provides the same CMake, host analysis tools, and BlocksDS
toolchain used by CI. No host compiler, CMake installation, BlocksDS SDK, GitHub
CLI, or development libraries are required.

On Windows, use Docker Desktop with Dev Containers or run the command-line
workflow through WSL.

### Visual Studio Code

Container creation configures both CMake presets and installs the repository's
Git hooks. The workspace also provides:

- clangd indexing and clang-tidy diagnostics for host and DSi sources
- clang-format on save for C files
- tasks for building, formatting, analysis, tests, and complete checks
- CodeLLDB debugging for the portable-core tests

Use **Run Build Task** to build the DSi ROM, **Tasks: Run Task** for other
workflows, and **Debug host tests** to launch the unit tests under CodeLLDB.
After changing CMake build definitions, run **DualSync: Configure IntelliSense**
to refresh both compilation databases.

### Command-line workflow

The `scripts/dualsync` entry point runs directly inside the Dev Container. From
a POSIX-compatible host shell, it starts the same environment through Docker
Compose v2:

```bash
./scripts/dualsync dsi
./scripts/dualsync test
```

Available commands:

- `configure`: generate the host and DSi compilation databases
- `dsi`: build the DSi ROM
- `host`: build the portable-core test executable
- `test`: run host formatting, analysis, and unit tests
- `check`: run all host checks and build the DSi ROM
- `format` / `format-check`: apply or verify C formatting
- `lint` / `tidy`: run cppcheck or clang-tidy
- `clean`: remove generated build artifacts
- `versions`: report the active host and DSi toolchain versions
- `install-hooks`: configure the pre-commit hook outside the Dev Container

Formatting follows an LLVM-derived C style with four-space indentation, Allman
braces, and a 100-column limit. CMake is the sole build definition, and
portable-core unit tests use cmocka through CTest.

The pre-commit hook checks formatting without modifying staged files. Hooks can
be bypassed, so CI always runs the complete check independently.

## DSi build

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

## Reproducibility and CI

The development image pins its base-image digest and direct package versions;
builds do not perform implicit upgrades. It currently uses the official
BlocksDS `slim` image for its maintained Wonderful and BlocksDS installation.
This is a bootstrap choice, not a platform boundary. When 3DS development
begins, devkitPro will be installed under its separate `/opt/devkitpro` prefix
so one container can support both applications.

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

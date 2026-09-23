# DualSync

DualSync is a native RomM client for Nintendo DS-family homebrew systems. The
first target is Nintendo DSi running through TWiLight Menu++; a Nintendo 3DS
client is planned later and should reuse the platform-independent RomM and save
sync logic.

This document defines product scope and an implementation sequence. It is not a
commitment to specific UI details or libraries until hardware spikes validate
them.

## Goals

The DSi client must support:

- Launching DualSync as DSi homebrew through TWiLight Menu++.
- Pairing with a RomM server without entering RomM credentials on the console.
- Browsing and searching the server's Nintendo DS library.
- Downloading Nintendo DS ROMs directly to the SD card.
- Associating each downloaded ROM with its RomM identity and local save path.
- Downloading, uploading, and synchronizing nds-bootstrap save files.
- Detecting save conflicts and resolving them without silently losing data.
- Recovering safely from interrupted downloads, syncs, and network failures.

The design should make a later 3DS client practical without forcing the DSi
implementation to use unsuitable 3DS abstractions.

## Non-goals

The initial DSi client will not include:

- Cover art.
- Cartridge dumping or cartridge save management.
- DSiWare downloading, installation, launching, or save management.
- Background synchronization.
- Emulator configuration management.
- Non-Nintendo DS RomM platforms.
- RomM administration or library scanning.
- A custom DualSync server or cloud service.

Directly launching a downloaded ROM from DualSync is not currently required.
The expected initial flow is to download or sync, exit DualSync, and launch the
game normally from TWiLight Menu++. A direct TWiLight/nds-bootstrap launch
handoff can be evaluated later.

## Product principles

1. **Never lose a save.** Destructive choices require confirmation, and every
   replaced local save gets a backup first.
2. **Pair rather than type credentials.** DualSync stores a revocable,
   device-bound token, never the user's RomM password.
3. **Stream everything.** ROMs, saves, and HTTP responses must not be buffered
   wholly in RAM.
4. **Design for interruption.** Losing Wi-Fi, closing the lid, or powering off
   must not leave a valid file replaced by a partial file.
5. **Keep the server authoritative for library metadata, not conflicts.** A
   conflict always requires an explicit user decision.
6. **Prefer direct RomM compatibility.** A companion bridge is a fallback only
   if DSi networking cannot be made reliable.
7. **Be secure by default without removing user control.** TLS verification is
   enabled by default. A user may explicitly disable it for an individual
   self-hosted server after acknowledging a clear interception warning.

## Target environment

### Initial target

- Nintendo DSi or DSi XL.
- DSi mode, to access the additional RAM and faster CPU.
- TWiLight Menu++ and nds-bootstrap.
- SD card storage exposed through libfat/DLDI-compatible filesystem APIs.
- RomM 5.3.0 or newer for the initial compatibility baseline.
- HTTPS with a publicly trusted certificate for the primary development server.

### Future target

- Nintendo 3DS-family systems.
- Homebrew Launcher `.3dsx` initially; CIA packaging can be considered later.
- Native 3DS networking and filesystem APIs.
- Nintendo DS saves managed as SD files.
- Native 3DS save archive support as a separate later scope.

## Proposed technical direction

### Language and build

- Portable C for the shared core.
- BlocksDS and Wonderful Toolchain for the first DSi implementation.
- libcurl plus mbedTLS if the HTTPS spike confirms acceptable memory use.
- A small C JSON parser with bounded allocations, initially cJSON unless
  measurement shows that a streaming parser is necessary.
- libctru and native `httpc`/`sslc` services for the later 3DS platform layer.
- Target-specific Docker images, orchestrated by a top-level Makefile.

Final dependencies must remain replaceable behind narrow interfaces.

### Source layout

```text
core/
  include/dualsync/
  source/
  tests/
apps/
  dsi/
    source/
    Makefile
  3ds/
    source/
    Makefile
containers/
  dsi/
    Dockerfile
  3ds/
    Dockerfile
build/
  obj/
  dist/
Makefile
```

The shared core owns protocol state machines and data rules. Platform code owns
network transport, rendering, input, clocks, filesystem primitives, and launch
integration. UI screens may share states and actions, but not rendering code.
The core is compiled separately by each target toolchain rather than distributed
as one precompiled ARM binary. All generated artifacts are kept under the
repository-level `build/` directory. Intermediate files are kept in
`build/obj/`, while files intended for distribution are copied to `build/dist/`.

## RomM integration

DualSync should use the public RomM API directly.

### Pairing

The preferred flow is RomM's device authorization flow:

1. The user enters the RomM server URL once.
2. DualSync verifies the server with a lightweight heartbeat request.
3. DualSync starts device authorization.
4. RomM returns a user code and verification URL.
5. DualSync displays the URL and code.
6. The user approves the device in a browser on another device.
7. DualSync polls at the interval requested by RomM.
8. DualSync stores the resulting token and device identity on the SD card.
9. On later launches, DualSync validates the token before loading the library.

The first version will use text-only pairing. QR display is a later enhancement
using the same verification URL. Pairing must handle pending, slow-down,
expiration, rejection, server incompatibility, revoked tokens, and retry.

The token file must not contain RomM credentials. It should be written through
a temporary file and atomic rename where the filesystem permits it.

### Server security settings

Each configured server records one TLS policy:

- **Verify certificates**: default and recommended.
- **Skip certificate verification**: allowed only after an explicit warning
  that credentials, tokens, ROMs, and saves could be intercepted or modified.

Disabling verification applies only to the selected server and remains visibly
indicated in server settings and connection status. DualSync must never disable
verification automatically after a certificate error.

Plain HTTP may also be used for a self-hosted LAN server after a warning. The
choice belongs to the user, but insecure transport must never be presented as
equivalent to verified HTTPS.

### Library browsing

The client will:

- Request only Nintendo DS platform entries.
- Fetch ROMs in small server-paginated pages.
- Support text search through RomM's server-side search/filtering.
- Display a text-focused list containing title and essential status.
- Fetch details only when a title is selected.
- Avoid retaining the complete library in memory.

Sorting can initially be limited to title. Additional server-supported sorting
should be added only if it is useful on the constrained UI.

### ROM downloads

Downloads will:

- Stream directly to a `.part` file using a fixed-size buffer.
- Check available SD space before starting when file size is known.
- Display transferred bytes and total size when available.
- Preserve the partial file after recoverable failures.
- Attempt HTTP Range resume only after confirming server support.
- Validate the final byte count and any server-provided integrity metadata.
- Rename the `.part` file only after validation succeeds.
- Persist the RomM ROM ID, server identity, destination path, and expected save
  path in DualSync's local manifest.

The destination root should be configurable, with a TWiLight-friendly default.
DualSync must not overwrite an unrelated existing ROM without confirmation.

The initial default paths are:

```text
sd:/roms/nds/<ROM filename>.nds
sd:/roms/nds/saves/<ROM filename without extension>.sav
```

These defaults must remain configurable because TWiLight installations and user
layouts can differ.

## Save model

### Local save discovery

The expected initial save type is an nds-bootstrap `.sav` file on the SD card.
DualSync should not scan the entire SD card on every sync. It should derive and
record the save path when a ROM is downloaded.

Save-path handling must account for:

- ROMs and saves stored in sibling or configured directories.
- User-renamed ROM files.
- Missing saves for games that have never created one.
- Existing ROMs that were not downloaded through DualSync.
- Case differences and filesystem path limits.

An explicit "link existing game" workflow can be added after downloads and save
sync work end-to-end. The manifest should be designed so this does not require
a format migration. Existing-ROM linking is not required for the first save-sync
release.

### Sync identity

Each managed save needs at least:

- RomM server identity.
- RomM ROM ID.
- RomM save slot, if applicable.
- Local ROM path.
- Local save path.
- Last successfully synchronized content hash.
- Last observed server save identity/hash if supplied by RomM.
- Last completed sync time for display and diagnostics only.

Content hashes, not timestamps alone, determine whether content changed.
Timestamps may assist RomM negotiation but must not silently resolve conflicts.
Hashing must be incremental.

### Sync flow

For each managed game:

1. Locate the expected local save.
2. Hash it incrementally if present.
3. Submit local state through RomM's sync negotiation API.
4. Handle RomM's `no_op`, `upload`, `download`, or `conflict` decision.
5. Transfer the save through a temporary file or streaming upload.
6. Back up any local save before replacing it.
7. Validate a downloaded save before replacement.
8. Report the result to RomM when the API requires session completion.
9. Update local sync metadata only after every required operation succeeds.

Initial sync is always user-triggered. The UI should offer:

- Sync all managed saves.
- Sync the selected game's save.
- Download a server save when no local save exists.
- Upload a local save when no server save exists.

### Conflict resolution

A conflict means both local and server content changed since the last successful
sync, or RomM otherwise cannot select a safe direction.

The conflict screen will show:

- Game title.
- Whether local and server copies exist.
- Available size, hash abbreviation, and timestamp metadata.
- A warning that choosing a side replaces the active copy.

Actions:

- **Keep local:** back up relevant files as needed, then upload local.
- **Keep server:** back up local, download server to a temporary file, validate,
  and replace local.
- **Skip:** make no content or sync-baseline changes. This is the default.

If RomM supports retaining both copies as independent save slots, a later
version may expose that operation. The MVP must not invent slot behavior that
the server cannot represent consistently.

### Backups

Before replacing a local save, copy it to a DualSync-owned backup directory.
Backups should include a stable game identifier and timestamp in their metadata,
without depending solely on the human-readable game title.

DualSync retains the five most recent backups per game by default. The user may
configure any non-negative retention count, including `0` to disable retained
backup history. After a successful replacement, backups older than the selected
limit are removed oldest-first.

A retention count of `0` does not disable transactional safety. DualSync must
still preserve a temporary rollback copy until the replacement is fully written,
validated, and committed. It may remove that rollback copy only after success.

## Local persistence

DualSync needs a small versioned configuration and state directory containing:

- Server URL, stable server identity, and TLS policy.
- Device identifier and paired token.
- Configured ROM and save roots.
- Download manifest.
- Sync baselines.
- Partial download metadata.
- Save backups.
- Diagnostic log with a bounded size.

State files should include an explicit schema version. Updates should use
write-temporary-then-rename behavior. A corrupt state file must produce a clear
error rather than silently resetting pairing or sync history.

## User experience

The initial screen model is:

1. **Server setup:** URL, TLS policy, connection test, and pairing.
2. **Library:** paginated Nintendo DS games and search.
3. **Game details:** local status, download, resume, and save sync.
4. **Downloads:** active/partial downloads and errors.
5. **Save sync:** managed saves, sync all, progress, and conflicts.
6. **Settings:** server, paths, token reset, and diagnostics.

The top screen should prioritize status and list content. The touch screen can
provide actions and text input. Every network operation needs visible progress,
a timeout, and a cancel path.

## Delivery phases

### Phase 0: Hardware and transport spikes

Deliverables:

- Minimal DSi-mode application launched through TWiLight Menu++.
- Wi-Fi initialization with timeout, retry, and cancel.
- Verified HTTPS request to a real RomM server.
- A second test using skipped verification, if needed for the user's server.
- Recorded executable size, idle memory, TLS handshake peak memory, request
  peak memory, and stable transfer buffer size.
- Streaming file download to SD without buffering the response.

Exit criteria:

- Repeated requests succeed on real DSi hardware under the configured policy.
- Certificate failures are distinguishable from other connection failures.
- Errors return control to the UI rather than hanging or crashing.
- Enough memory remains for JSON parsing and a basic UI.

If this fails, evaluate in order:

1. Reducing TLS and HTTP feature configuration.
2. Smaller buffers and a streaming JSON parser.
3. User-selected LAN HTTP with explicit warnings.
4. A narrowly scoped local HTTPS-to-HTTP bridge.

### Phase 1: Pairing and session persistence

Deliverables:

- Server URL entry, TLS policy, and heartbeat.
- Text-based device pairing.
- Token persistence, validation, revocation handling, and unpair.
- Clear TLS and server compatibility errors.

Exit criteria:

- A new installation can pair without entering a RomM password.
- Restarting DualSync preserves a valid session.
- Revoking the token in RomM returns the client to pairing safely.

### Phase 2: Nintendo DS library browser

Deliverables:

- Nintendo DS platform selection.
- Paginated text library.
- Search and basic title details.
- Loading, empty, timeout, and retry states.

Exit criteria:

- A large library can be browsed without memory growth across many pages.
- Search results come from the server rather than a full local index.

### Phase 3: ROM downloads

Deliverables:

- Configurable destination root.
- Streamed downloads, progress, cancellation, and partial files.
- Resume where supported.
- Free-space and overwrite checks.
- Persistent ROM-to-save manifest.

Exit criteria:

- A large ROM downloads without memory growth.
- Interrupted downloads never appear as completed ROMs.
- A resumed or restarted download produces the correct final file.
- TWiLight Menu++ can launch a downloaded ROM normally.

### Phase 4: Save download and upload

Deliverables:

- nds-bootstrap save-path mapping.
- Incremental hashing.
- Per-game upload and download.
- Temporary files, validation, and pre-replacement backups.
- Initial sync behavior for one-sided saves.

Exit criteria:

- A save can move from DSi to RomM and back without content changes.
- Failure at any transfer stage preserves the previous active local save.
- Missing local and remote saves are reported accurately.

### Phase 5: Full synchronization and conflicts

Deliverables:

- RomM sync negotiation.
- Sync-all workflow.
- Local/server/both-changed detection.
- Keep-local, keep-server, and skip conflict actions.
- Persistent last-synced baselines.
- Recovery after interruption during a batch.

Exit criteria:

- Unchanged saves cause no transfer.
- One-sided changes choose the safe automatic direction.
- Two-sided changes never overwrite either side without explicit confirmation.
- Skipped conflicts remain unresolved and are shown again later.

### Phase 6: Usability and hardening

Potential work:

- QR-code pairing.
- Bounded backup retention.
- Link existing ROMs.
- Direct handoff to TWiLight Menu++ or nds-bootstrap.
- Better diagnostics and exportable logs.
- Localization-ready strings.
- Compatibility testing across RomM and TWiLight Menu++ versions.

### Phase 7: 3DS client

Reuse:

- RomM models and API serialization.
- Device pairing state machine.
- Pagination and search logic.
- Download state machine.
- Save negotiation and conflict rules.
- Versioned persistence formats where filesystem semantics permit.

Replace:

- Network transport with libctru `httpc`/`sslc`.
- Rendering and input with 3DS-specific UI code.
- Filesystem and launch integration.
- Save discovery for native 3DS titles, if that scope is adopted.

The initial 3DS release may deliberately support only the same Nintendo DS ROM
and nds-bootstrap save workflow as DSi. Native 3DS title/save management should
be treated as a separate feature because it requires archive APIs and different
identity rules.

## Testing strategy

Shared core logic should be buildable and testable on a desktop host:

- Pairing response and polling state tests.
- Paginated API parsing tests using captured sanitized fixtures.
- Download resume and temporary-file state tests.
- Save hash and sync-decision tests.
- Conflict action tests.
- Persistence corruption and schema migration tests.

Hardware testing is required for:

- Wi-Fi initialization and failure modes.
- TLS memory and certificate behavior.
- SD write performance and interruption.
- TWiLight Menu++ launch compatibility.
- Actual nds-bootstrap save paths.
- Long-running memory stability.

melonDS may speed up UI and filesystem iteration, but it is not a substitute for
real DSi testing of Wi-Fi, TLS memory usage, or SD failure behavior.

## Major risks

| Risk | Impact | Mitigation |
| --- | --- | --- |
| TLS exceeds practical DSi memory | Blocks direct HTTPS access | Make it the first hardware spike; keep transport replaceable |
| RomM API changes between releases | Pairing or sync breaks | Detect server version/capabilities and isolate API adapters |
| TWiLight save paths vary | Wrong or missing save selected | Persist explicit paths and validate against real installations |
| Power/network loss during replacement | Save or ROM corruption | Temporary files, validation, backups, atomic rename |
| Timestamp semantics differ | False conflict decisions | Use content hashes and RomM negotiation |
| Large JSON responses fragment memory | Crashes while browsing | Small pages, response limits, streaming parser fallback |
| Token or state file corruption | Pairing or sync baseline loss | Versioned files, atomic writes, explicit recovery UI |

## Decisions still needed

These decisions do not block Phase 0, but should be settled before their
respective features:

1. **Self-signed certificate options:** skipping verification is supported, but
   optional CA import or certificate fingerprint pinning could provide safer
   alternatives later.
2. **Direct game launch:** remain an exit-to-TWiLight workflow or add launch
   handoff after downloading/syncing. Recommendation: defer until all sync
   safety work is complete.

## Recorded decisions

- **Initial RomM baseline:** RomM 5.3.0. Older-server fallbacks are not required;
  incompatible servers should receive a clear error.
- **Primary development connection:** verified HTTPS using a publicly trusted
  Let's Encrypt certificate.
- **Connection flexibility:** per-server HTTP and skipped TLS verification are
  allowed only after explicit warnings.
- **Initial ROM path:** `sd:/roms/nds/<ROM filename>.nds`.
- **Initial save path:** `sd:/roms/nds/saves/<ROM filename without extension>.sav`.
- **Existing ROM linking:** deferred until managed downloads and save sync work
  end-to-end.
- **Backup retention:** retain the newest five backups per game by default,
  configurable to any non-negative count. `0` disables retained history but not
  the temporary rollback copy required during replacement.

## Immediate next steps

1. Bootstrap a minimal BlocksDS project that runs in DSi mode.
2. Implement a verified HTTPS request to the RomM 5.3.0 heartbeat endpoint.
3. Measure the Phase 0 heartbeat/TLS spike on real hardware.
4. Stream a test download into `sd:/roms/nds/` through a temporary file.
5. Use the measurements to finalize the HTTP, TLS, JSON, and buffer choices.

# XGBA P0: Extended ROM design and acceptance plan

Status: P0.0 and P0.1 implemented and tested; P0.2 CPU data-read implementation added, pending its exact-head CI result. Instruction execution, DMA and end-to-end testing are not yet implemented.

## Purpose

XGBA is an experimental, public fork of mGBA that aims to run standard Game Boy Advance software unchanged and additionally support opt-in custom ROM images with up to **256 MiB of distinct, directly addressable ROM data**. This is a virtual hardware extension, not a claim of compatibility with physical GBA hardware.

Do not modify `DevElectrum/grand-anthology` during XGBA P0.

## Proposed address map

- Legacy ROM: `0x08000000` through `0x09FFFFFF`, 32 MiB window backed by the first 32 MiB of the ROM.
- Extended ROM: `0x10000000` through `0x1FFFFFFF`, 256 MiB window backed by ROM offsets `0x00000000` through `0x0FFFFFFF`.
- The extended window's first 32 MiB aliases the legacy content; it must not duplicate ROM bytes in the file.
- Original cartridge mirrors and all ordinary GBA memory regions retain their legacy behavior.
- Extended data is read-only and is accessible using normal emulated 32-bit addresses. No cartridge banking. The emulator keeps an independent backing map so legacy cartridge copy-on-write does not accidentally unmap the expanded data. Patching a legacy ROM and preserving alias consistency still require integration review.
- Region detection and activation are **opt-in**: ROMs larger than 32 MiB with ASCII marker `XGBA256` at header offsets `0xB5..0xBB` use the expanded mapping. Tagged images over 256 MiB are rejected. Untagged oversized files retain upstream behavior. The marker occupies the GBA header's seven reserved bytes; a ROM-builder must recalculate the header checksum after changing those bytes.
- This is a 256 **MiB** target (268,435,456 bytes), not 256,000,000 bytes.

## Invariants

1. A normal GBA ROM loads, executes, and saves exactly as in upstream mGBA.
2. Extended ROM backing, byte/halfword/word reads, and instruction fetches are all explicitly bounds-checked.
3. ROM load/unload/reset handle backing-memory lifetime without leaks, invalid unmaps, or stale CPU pointers.
4. The CPU can execute ARM and Thumb instructions from extended addresses after the appropriate fetch-path changes.
5. Extended-ROM DMA support must be considered separately because upstream DMA masks and address validity rules reject those addresses.
6. Virtual memory/debugger inspection, patching, savestate restore, hashing, and timing must be reviewed for extended addresses.
7. Additional working RAM, enlarged SRAM/save files, improved graphics, and multi-gigabyte ROMs are not P0 requirements.

## Implementation order and gates

### P0.0: Baseline CI (complete)

- Headless Linux build, unit tests, and reproducible build settings in GitHub Actions.
- Baseline succeeded: run 41004425993.

### P0.1: Loader (complete)

- Extended backing, safe unloading and 32 MiB legacy mapping are implemented.
- New unit tests load and unload a synthetic 256 MiB image, check legacy behavior for an unmarked 64 MiB image, and reject tagged images over 256 MiB.
- The test-inclusive CI run 41005025428 passed.

### P0.2: CPU data access (in progress)

- Bounded 8-, 16-, and 32-bit extended ROM loads were added to the CPU memory handlers, preserving legacy handling outside the opt-in address range.
- Added checks for the 32 MiB boundary, midpoint, and last aligned word using CPU load handlers.
- Additional checks still needed for unaligned loads, short images, out-of-bounds reads, legacy mirrors, and hardware timing.

### P0.3: Instruction fetching

- Extend active instruction region selection and prefetch/timing paths.
- Exercise ARM and Thumb code from the extended ROM with deterministic results.

### P0.4: Integration and regressions

- Address DMA support, debugger/core memory blocks, resets, savestate correctness, and existing cartridge behavior.
- Run automated old-ROM and extended-ROM tests separately.

### P0.5: Integration ROM and developer release

- Build a standalone, synthetic, redistribution-safe 256 MiB GBA test ROM at CI runtime.
- Create a separate Emerald-based integration project, with independent map references and a region-switching hub, using clean source assets and legally supplied build inputs.
- Start with one cloned area and incrementally reach the **Twenty Hoenns** scenario.
- Hoenn clones should use independently stored map/layout data when measuring storage; shared graphics alone do not consume new ROM bytes.
- Where necessary, add deterministic non-game data to exercise the entire 256 MiB range.
- Automated navigation, data verification, clean restart, and optional interactive gameplay are final acceptance gates.

## Source audit (fork baseline)

- `src/gba/gba.c`: ROM load/unload maps and unmaps conventional 32 MiB.
- `src/gba/memory.c`: current CPU loads and active instruction region use original ROM regions and masks.
- `include/mgba/internal/gba/memory.h`: ROM region definitions and backing fields.
- `src/gba/dma.c`: DMA address masks reject extended sources.
- `src/gba/core.c`: memory blocks exposed to debugger/frontend.
- `src/gba/serialize.c`: savestate size and CPU state checks.
- `src/gba/test/core.c`: existing CMocka test suite.
- `src/platform/test/CMakeLists.txt`: CTest integration.

## Repository policy

- Work on feature branches; do not change upstream-facing `master` until verified.
- Preserve original mGBA copyright statements and MPL-2.0 notices.
- Build large generated ROMs on CI runners. Do not commit 256 MiB images or proprietary ROM dumps.
- Never introduce changes into Grand Anthology as part of the XGBA P0 experiment.

# XGBA P0: Extended ROM design and acceptance plan

Status: Synthetic P0 core validation passed on the validation branch, including partial tagged ROM execution, memory views, waitstate comparisons and SDL compilation. Full Emerald/Hoenn gameplay integration and broader compatibility remain open. See [2026-10-30 validation report](XGBA_P0_VALIDATION_2026-10-30.md) and [CI run #90](https://github.com/DevElectrum/xgba/actions/runs/41014731546).

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
- New unit tests load and unload a synthetic 256 MiB image, check legacy behavior for an unmarked 64 MiB image, and reject tagged images over 256 MiB. Linux CI also constructs a sparse 256 MiB file on disk and tests its actual VFile mapping, last-word CPU read and unload.
- The test-inclusive CI run 41005025428 passed.

### P0.2: CPU data access (implemented; extended regressions ongoing)

- Bounded 8-, 16-, and 32-bit extended ROM loads were added to the CPU memory handlers, preserving legacy handling outside the opt-in address range.
- The ARM LDM block-load path now supports extended ROM reads.
- CI verifies the 32 MiB boundary, midpoint, last aligned word, unaligned load rotation, and block loads using CPU handlers. The scalar-read run 41005441475 and block-load run 41005565372 passed.
- The real 256 MiB file-mapping test passed in run 41005662564.
- CI also verifies a tagged 64 MiB image, its last word, and a legacy/extended byte alias (run 41006766044). Scalar out-of-bounds reads and partial-size CPU execution were additionally exercised on the validation branch. Comprehensive real-game compatibility remains unverified.

### P0.3: Instruction fetching (full-size and partial tagged images validated)

- Full-size (exactly 256 MiB) opt-in images can execute ARM and Thumb instructions directly from the `0x10000000..0x1FFFFFFF` range.
- CI runs a real ARM instruction above the conventional 32 MiB limit and a Thumb instruction near the end of extended space; run 41006152418 passed.
- The validation branch implements a safe padded CPU backing for partial-size opt-in ROMs, with a 64 MiB ARM/Thumb execution and savestate regression. Explicit jumps beyond the loaded length are rejected. Sequential execution into the padded tail needs additional semantic validation.
- Sequential ARM instruction prefetch across 32 MiB and ROM0-equivalent data-read waitstate comparisons now pass; deeper timing, prefetch and boundary regressions are still appropriate.

### P0.4: Integration and regressions (partially implemented)

- DMA1–DMA3 source-address handling preserves the expanded address rather than applying original 28-bit masks. DMA0 retains its legacy limitation.
- Extended-ROM regions adopt original ROM0 waitstate timings when WAITCNT changes.
- CI runs real DMA3 events for 32-bit and 16-bit transfers into EWRAM (run 41006481250) and validation-branch DMA1/DMA2 32-bit transfers (run 41014731546).
- Core debugger memory-block listings expose extended ROM only when a tagged image is loaded, and `getMemoryBlock` returns its mapping (run 41006766044 passed).
- **Still open:** ROM patch/cheat alias consistency, broader real-game compatibility, accurate cycle timing and sequential fetch beyond partial-ROM EOF. Basic full-size and partial-size extended-ROM savestate roundtrips pass.

### P0.5: Integration ROM and developer release (synthetic cartridge complete)

- CI generates two copyright-free cartridges with `tools/xgba_generate_test_rom.py`: a 16 MiB legacy cartridge (`--legacy`) and a 256 MiB tagged XGBA cartridge.
- Both cartridges run a real guest ARM program starting at the conventional cartridge entry point, reading data from their respective ROM ranges and storing the result to EWRAM. The paired legacy and extended regression suite passed in run 41007971184.
- The extended cartridge can save state while executing instructions near the end of the 256 MiB range, restore EWRAM and CPU state, and resume ARM instructions (run 41007814854).
- A second guest ARM routine reads 20 records spaced across the extended 256 MiB image, sums their values and writes the checksum to EWRAM. CI verified the checksum in run 41008162819. These are synthetic distributed region records, **not cloned Hoenn maps**.
- A separate Emerald-based integration project remains to be built, with independent map references and a region-switching hub, using clean source assets and legally supplied build inputs.
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

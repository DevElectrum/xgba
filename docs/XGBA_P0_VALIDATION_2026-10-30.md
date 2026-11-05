# XGBA P0 validation report — 2026-10-30

## Scope and isolation

Validation branch: `xgba/p0-validation-20261030`, based on `xgba/p0-foundation`.
Draft validation PR: https://github.com/DevElectrum/xgba/pull/2
The `master` branch and `DevElectrum/grand-anthology` were not changed.

## Latest verified CI

**PASS:** https://github.com/DevElectrum/xgba/actions/runs/41014731546 (run #90).

- Headless GBA emulator and cmocka test build: passed.
- SDL2 desktop emulator **compilation**: passed (`build-xgba/sdl/mgba`).
- CTest: **16/16 registered suites passed** (including `gba-core`).
- Copyright-free 16 MiB ordinary GBA and 256 MiB XGBA synthetic cartridges: generated and tested.
- Existing 20-point distributed address probe, full-size savestate check, DMA3 and ARM/Thumb execution checks remain covered.

## New validation work

- Partial tagged ROMs (tested at 64 MiB) now use a zero-padded 256 MiB private backing buffer to make direct ARM/Thumb prefetch safe. An ownership flag ensures correct cleanup. Data reads are still bounded by the actual file length. This consumes a 256 MiB virtual mapping per partial cartridge, and the loaded part is copied into it.
- Executed ARM and Thumb instructions near the end of the 64 MiB ROM.
- Confirmed basic partial-ROM savestate save/restore, register restoration and subsequent execution.
- Confirmed that an explicit branch into absent data past the partial ROM limit is rejected by the emulator's invalid-PC path.
- Confirmed that out-of-bounds 8/16/32-bit scalar reads take the invalid-read path rather than indexing beyond the true file.
- Added debugger memory-view reads of the extended address window for 8/16/32-bit values.
- Executed sequential ARM instructions across the traditional 32 MiB ROM boundary.
- Compared ordinary ROM0 and expanded 32-bit data-read cycle counters and checked 16/32-bit sequential/nonsequential waitstate tables for all sixteen extended regions after WAITCNT adjustment.
- Confirmed real DMA1 and DMA2 32-bit transfers from expanded ROM to EWRAM; the existing suite checks DMA3 16/32-bit transfers.
- Confirmed the SDL2 frontend compiles, not that it can already run a game interactively.

## Known limits and deferred checks

1. **Extended-window runtime patching and cheat alias coherence are not complete.** The old `GBAPatch8/16/32` paths implement legacy ROM address cases; writes to a legacy ROM copy-on-write mapping do not automatically synchronize the expanded backing. `GBAApplyPatch` still limits output to conventional ROM size. Do not rely on emulator-side patching or ROM cheats during the Emerald integration experiment.
2. **Instruction fetch beyond partial-ROM EOF:** explicit out-of-range jumps are rejected and the padding prevents host out-of-bounds access; sequential execution into the padding has not been proven to stop with faithful open-bus semantics.
3. The waitstate checks establish **ROM0-equivalent internal timing**, not physical hardware equivalence for a virtual address extension.
4. Basic full-size and partial-size savestate roundtrips pass; compatibility across versions, save file formats, rewind, debugger breakpoints, and extended ROM patch-state combinations remain unverified.
5. Automated tests use synthetic cartridges, not an actual Emerald, FireRed, or other complete game. SDL compilation passed, but interactive video/audio/input and Android are untested.
6. The 20 distributed ROM probes are **not** twenty map copies. No Emerald ROM, `pokeemerald` source, or Grand Anthology repository was modified.

## Engineering decision

**Ready to begin an isolated Emerald integration experiment, not ready for a general XGBA emulator release.**

Use legally obtained / reproducible Emerald development inputs. First produce one small, independently stored map/layout payload physically placed beyond the 32 MiB limit, with a game-side reference that successfully reaches `0x10000000+` virtual space. Verify graphical rendering, event/script behavior, warps, clean boot, save/reload, and repeatable navigation. After the one-map smoke test succeeds, increase the number and size of independent Hoenn map copies toward the proposed Twenty Hoenns stress scenario. Keep generated large ROM binaries out of Git history and keep Grand Anthology isolated.

Do not merge the validation PR into `master` until broader compatibility and interactive tests have passed.

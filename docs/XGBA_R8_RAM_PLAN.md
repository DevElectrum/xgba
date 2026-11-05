# XGBA R8: isolated 4 MiB RAM capability (experimental)

Branch: `xgba/r8-4mib-ram` from frozen E7 `9aa5e11e`. No production release and no changes to Anthology, Mosaic or player saves.

## Contract
- ROM opt-in: E6 256 MiB tagged ROM and exact 16-byte descriptor at offset `0x0FFFFF00`: ASCII `XGBARAM8`, little-endian version 1, size 4194304.
- RAM window: `0x20000000..0x203FFFFF`, exactly 4 MiB, volatile and zeroed at reset. Native 256 KiB EWRAM, 32 KiB IWRAM, 256 MiB ROM and 16 MiB E6 save remain unchanged.
- Direct CPU reads, writes, ARM/Thumb instruction fetch and multi-load/store through explicit window mapping; unsupported unmapped addresses do not alias RAM.
- DMA3 may access the window. DMA0/1/2 requests involving it are rejected, not truncated.
- Core state size conditionally includes the complete RAM with a versioned 16-byte header and CRC32. A corrupt or absent R8 payload must fail state loading before modifying the running state.
- ROM load failure or allocation failure must fail cleanly. An untagged ROM gets no RAM allocation.
- The capability must not be confused with disk persistence. A2's independent journal remains authoritative for Pokémon and inventory.

## Adversarial gates
1. CPU 8/16/32-bit boundary test, full 4 MiB pattern sweep, ARM/Thumb, LDM/STM, debugger view.
2. DMA3 round-trip both directions, exact high-address preservation, unsupported DMA channel rejection.
3. Reset clearing, new ROM unload clearing, opt-in descriptor rejection and compatibility tests for old ROMs.
4. Rewind and savestate state payload preservation, corruption rejection and legacy state-size unchanged for ordinary ROMs.
5. Entire existing E5/E6/E7 CTest suite; no gameplay ROMs or user saves uploaded by CI.

CI: `.github/workflows/xgba-r8.yml` on the R8 branch. This plan is not an A2 release or gameplay acceptance.

## R8.1–R8.3 implementation and audit outcome (2026-11-02)

Full core suite GREEN: https://github.com/DevElectrum/xgba/actions/runs/41439577345 (16/16 suites including 14 gba-core tests). Validated code HEAD: `979c2470aa7068ff7a7299718585267207d54195`. Source includes RAM decoder, allocation/reset/unload, CPU, DMA3, debugger mapping, versioned savestates, rewind and direct adversarial tests.

- Required ROM descriptor bytes: `XGBARAM8` (8 bytes), `01 00 00 00` (version), `00 00 40 00` (4 MiB size), at ROM file offset `0x0FFFFF00`. R8 enabled only for E6 ROM revision and full 256 MiB XGBA256 mapping. The existing E7 final 16-byte ROM probe is not overwritten. Invalid recognized descriptor fails ROM load.
- CPU RAM outside `0x20000000..0x203FFFFF` must fail, including instruction fetch, instead of mirroring. Ordinary GBA EWRAM remains 256 KiB and IWRAM 32 KiB. Unmodified ROMs do not enable R8.
- DMA3 supports R8 as source or destination with exact 32-bit addresses. Other channels and out-of-window source/destination are rejected before activation.
- The core's raw savestate grows by `4194304 + 16` bytes on R8 ROMs only. The 16-byte header includes magic `0x38524158`, version, size and CRC32. Rewind uses named core states; its complete 4 MiB roundtrip passed. A missing/corrupt R8 payload is rejected prior to replacing running RAM. **Do not attempt to open an old 256 KiB-only savestate from a future RAM-dependent Anthology build.** A2 must use verified in-game E6 persistence and restart to reconstruct its cache.
- A bug in the bit-rotate macro argument would return altered high bits even though the backing RAM bytes were correct; fixed by supplying a named rotate variable. Out-of-range CPU fetch mirroring and high-bit DMA truncation were also addressed.
- Headless emulator CI is not a measured 5x gameplay benchmark. 4 MiB state/rewrite is intentionally limited to savestate and rewind operations, not every gameplay frame. Performance/latency after actual Anthology integration is still OPEN, and should be measured in one isolated CI benchmark before gameplay acceptance.
- No ROM or save uploaded by CI, and no Nyx or existing player files touched. R8.1–R8.3 = **emulator-source and automated test accepted**, A2 RAM integration = **not started**, gameplay performance acceptance = **pending**.

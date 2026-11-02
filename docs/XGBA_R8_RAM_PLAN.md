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

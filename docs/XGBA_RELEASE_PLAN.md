# XGBA v0.1.0 desktop release engineering

Active working branch: `release/xgba-v0.1.0`, preserved base `xgba/e7-accepted-20261031`.

## Scope

- Rebrand visible product identity and terminal commands to XGBA, using the approved purple handheld XGBA artwork. Retain upstream originals for license/provenance only.
- SDL executable `xgba`, Qt executable `xgba-qt`, headless optional `xgba-headless`; build target `xgba-sdl`.
- Separate config and package IDs from upstream mGBA. Keep upstream MPL-2.0 attribution and internal include/library ABI intact.
- Disable mGBA's update mechanism and do not display untranslated upstream localized XGBA branding.
- Preserve 256 MiB XGBA ROM and E5/E6 optional save semantics unchanged.

## Automated release qualification

1. Baseline emulator and XGBA256 synthetic high-address reads, DMA, savestates, and E5/E6 persistence: `.github/workflows/xgba-ci.yml`.
2. Linux Qt GUI compilation and independent executable check: `.github/workflows/xgba-qt.yml`.
3. Linux release job: build Qt, SDL, tests; run ctest and create Debian package artifacts; publish SHA-256 checksum.
4. Windows release job: install Qt 6.8.3 MSVC 2022, compile Qt app, deploy Qt DLLs, create portable ZIP with license and checksums. Packaging is a **release candidate**, not signed/stable until green CI and real app launch.

## Acceptance gate before tagging

- Green latest CI on release branch, not an older commit.
- A locally launched fresh XGBA Qt app reports XGBA/version, opens ROM file, inputs/audio and fast-forward work; an ordinary GBA cartridge boots.
- An XGBA256 cartridge accesses high-ROM geometry, saves a disposable copied 128 KiB game save, closes and relaunches without corruption.
- Qt GUI executable, Linux desktop integration and Windows ZIP must match XGBA names. Visible logo and native application icons show the approved XGBA purple handheld artwork.
- Confirm that only XGBA owns its config directory, bug report URL and update behavior. Old mGBA and E5/E6 saves are left untouched.
- Tag and GitHub publish `v0.1.0` only after these checks. Do not distribute user ROMs or copyright-protected BIOS content.

## Known follow-on scope

Portable AppImage, Windows graphical installer, release signing/notarization, update feeds and retranslated locales may follow release-candidate qualification. They are not demonstrated by merely building the renamed Qt executable.

Native XGBA icons are generated from the committed 128px approved handheld icon. The Windows installer uses the generated XGBA icon and default Inno Setup wizard artwork instead of upstream mGBA installer artwork.

## Linux install launcher contract

On Linux, `xgba` is a tiny executable shell launcher installed with the Qt package and using `exec xgba-qt "$@"`; `xgba-qt` remains available directly. SDL installs as `xgba-sdl` so installing both frontends cannot overwrite the GUI launcher. Desktop `Exec=xgba %f` uses the same entrypoint. No in-tree build executable alias is implied, only installed commands. Install with `sudo cmake --install build-release` after a fresh CMake Release configure and full build, then `sudo ldconfig`. CI verifies staged executable paths and the launcher file.

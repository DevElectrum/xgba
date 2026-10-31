# XGBA living handoff for future Astras

## 2026-10-31: E5 emulator persistence regression and safer detection

- Active E5 branch `xgba/e5-save-expansion` now at commit `0a0a1e5ff82ba807944612dec3f8463cbda3b2c5`.
- Full v1 E5 superblock validation now checks the 24-byte header (magic, version, 128 KiB native size, 1 MiB size, CRC32) and accepts a surviving duplicate at 0xFF000. Simple 8-byte magic match is no longer enough to enable 0x0F writes.
- New `GBACore.e5SavedataPersistent` test creates an E5 ROM and temp 1 MiB save, writes through CPU 8-bit `0x0F` access, checks bounds, unloads/reopens the save and verifies persistence, tests backup superblock recovery, and verifies E5 maps fail closed after both headers are damaged.
- **Verified:** green full emulator CI: https://github.com/DevElectrum/xgba/actions/runs/41172551382 (16/16 CTest suites).
- Emerald integration work exists separately on `DevElectrum/xgba-emerald-test@xgba/e5-expanded-save`, with guest API plus NPC writer/auditor. Do not declare full E5 user acceptance until Emerald's current ROM build is green and a copy-only gameplay save/restart acceptance test is completed.
- No edits to master, no Grand Anthology changes, no modifications of user save files.

Updated: 2026-10-30

## E5 opt-in expanded save bridge started (2026-10-30)

- User completed E4 world-independent event prototype, World 20 persisted 3/SET and World 3 persisted 1/SET across normal game save and emulator exit/restart. User requested to start E5 and went to bed; **do not ask for tests until E5 in-game API is ready**.
- E5 emulator experimental branch: `xgba/e5-save-expansion` from accepted `xgba/p1-fast-forward`, draft [PR #4](https://github.com/DevElectrum/xgba/pull/4). E5 game branch `DevElectrum/xgba-emerald-test@xgba/e5-expanded-save`, draft [PR #6](https://github.com/DevElectrum/xgba-emerald-test/pull/6). Never touch `DevElectrum/grand-anthology`.
- Fully validated native save-sector audit: every sector in Emerald's 128 KiB / 32-sector flash is occupied: main save slots 0-27, Hall of Fame 28-29, Trainer Hill 30, recorded battle 31. E5 must not steal any native sector or enlarge the native SaveBlock1.
- E5 ROM requires existing `XGBA256` 0xB5..0xBB tag, revision byte `0xBC = 0xE5` and corrected GBA checksum. This prevents E4 ROMs from activating the storage bridge.
- Emulator `src/gba/savedata.c` now maps `1 MiB` for Flash1M when a tagged E5 ROM AND an already initialized 1 MiB save with `XGBAE5S\0` superblock at file offset 0x20000 are present. If save is short or uninitialized, it refuses extended mapping and keeps standard flash; it will not truncate/move an old E4 save or its RTC metadata. The standard flash commands remain unchanged at 0x0E and bank 0/1 of original 128KiB.
- Emulator `src/gba/memory.c` maps CPU addresses `0x0F000000..0x0F0DFFFF` as a linear 896 KiB persistent extension corresponding to appended `.sav` offsets `0x20000..0xFFFFF`. E5-only writes mark savedata dirty so mGBA's regular sync writes them. Out-of-range addresses in 0x0F return FF/drop writes in E5 mode. For non-E5 ROMs, 0x0F remains normal SRAM mirror. Direct byte read/write path is prototype; no user runtime test yet.
- mGBA's normal RTC may append a 16-byte trailer after all mapped save bytes. An E5 save can be `1 MiB + 16 bytes`; `GBASavedataRTCRead/Write` uses the expanded size and retains the trailer at the new end. Corresponding host-format tool migrates 128 KiB (+ optional 16 B RTC) into E5 format without mutating the original.
- **CI PASSED**: [E5 emulator build Actions #41041284439](https://github.com/DevElectrum/xgba/actions/runs/41041284439) compiled the full headless/SDL GBA build and ran inherited emulator tests, including fast-forward regression. This validates compilation and existing tests, **not yet new 0x0F persistence behavior in a running game**.
- **Companion game CI PASSED**: [E5 ROM Actions #41041353411](https://github.com/DevElectrum/xgba-emerald-test/actions/runs/41041353411) compiled 64 MiB E5-marked Emerald and checked revision and checksum, alongside offline-format tests. Preserve accepted E3/E4 ROM and save copies.
- Next: game-side E5 header/feature probe and safe world-bank read/write protocol, plus integration test proving 0x0F writes persist across normal in-game save / XGBA exit / reload. Add automated emulator test of 0x0F region if feasible. Avoid falsely declaring E5 accepted before those tests.


## User-verified and GUI preference (2026-10-30)

- **USER CONFIRMED**: Tab fast-forward now works in the SDL XGBA build from the `xgba/p1-fast-forward` branch, with the 64 MiB Emerald ROM. This clears the user-facing SDL turbo acceptance gate, although there is no measured benchmark.
- User also expects the **full mGBA menu-driven desktop GUI**, not the minimal SDL game window. Explain that prior build explicitly set `BUILD_QT=OFF`, then guided them to launch `build-xgba/sdl/mgba`. The menu frontend is Qt, built from the *same* XGBA core by enabling `BUILD_QT=ON` and providing Qt development libraries. **Do not confuse this with losing mGBA functionality or delete working SDL turbo.**
- Qt build and its menu/input/fast-forward behavior **have not yet been verified** on Nyx. Prefer building both SDL and Qt and explicitly checking hold-Tab turbo in Qt as well; Qt has its own hotkey/settings code, not necessarily the SDL Tab handler.
- Expected Qt executable: `build-xgba/qt/mgba-qt` (verify actual build output). CMake Qt frontend requires Qt Widgets/Network, with Qt Multimedia or SDL audio; Qt6 also needs OpenGL + OpenGLWidgets packages.
- Next: guide user through Qt dependencies / reconfigure while retaining SDL and its working turbo; keep living handoff current.

## Qt GUI configuration failure and fix (2026-10-30)

- User requested menu-rich mGBA GUI. Previously recommended Qt6 configure stopped at `src/platform/qt/CMakeLists.txt:506` with `string sub-command REPLACE requires at least four arguments`, then Ninja could not find `build.ninja`; executable `build-xgba-gui/qt/mgba-qt` never existed.
- **Root cause:** no Git release tags in this fork; unquoted `\${RELEASE_TAGS}` expanded to zero arguments in `string(REPLACE "\\n" ";" TAGS_LIST \${RELEASE_TAGS})`. This is a source CMake bug, **not** evidence that the Qt libraries were missing. An absent optional Qt6LinguistTools package also warned but was not the fatal CMake error.
- **Fix committed:** `daaf7f7e0947320c4030d4b8d99ddc23fd16731f` quotes the interpolation `"\${RELEASE_TAGS}"` to permit an empty tag list.
- **Qt compilation coverage:** `.github/workflows/xgba-qt.yml` was added in commit `447e8e983215fa5afe4369748b6a88e09e1806b6`, installing Qt6 dev libraries and compiling `mgba-qt` in CI. Check https://github.com/DevElectrum/xgba/actions/workflows/xgba-qt.yml and wait for green before claiming verified compilation.
- **Immediate workaround** without source update: pass `-DSKIP_GIT=ON` when configuring Qt. This skips optional release-tag metadata processing.
- **Local recommended fix:** `cd ~/Projects/xgba && git pull --ff-only origin xgba/p1-fast-forward`, then rerun the Qt6 configure and build; keep `BUILD_QT=ON`, `BUILD_SDL=ON`, `FORCE_QT_VERSION=6`, install `qt6-base-dev qt6-multimedia-dev libqt6opengl6-dev qt6-l10n-tools` if needed. Do not run Ninja after a failed configure.
- User confirmed **Tab fast-forward works in the SDL build**. Qt turbo behavior still requires a separate manual smoke test after the Qt UI launches.

## State and repositories

- Emulator `DevElectrum/xgba`, `master` remains untouched.
- Original emulator foundation: `xgba/p0-foundation`, [draft PR #1](https://github.com/DevElectrum/xgba/pull/1).
- Validated CPU/mapping branch: `xgba/p0-validation-20261030`, [draft PR #2](https://github.com/DevElectrum/xgba/pull/2), verified 16 CTest suites and SDL compilation in [Actions #94](https://github.com/DevElectrum/xgba/actions/runs/41014962808).
- Current **mandatory fast-forward fix** branch: `xgba/p1-fast-forward`, [draft PR #3](https://github.com/DevElectrum/xgba/pull/3) targeting the validation branch. Code implementation verified with **green XGBA Actions #41020527790** (https://github.com/DevElectrum/xgba/actions/runs/41020527790), including emulator tests, SDL build, and static fast-forward contract. **Do not declare speed fixed until the user tests it in actual Emerald gameplay.**
- ROM test repo: `DevElectrum/xgba-emerald-test`, branch `xgba/e0-expanded-layout`, draft PR #1. User confirmed the generated 64 MiB Emerald ROM **works fine** interactively in XGBA. Build/ROM-integrity CI passed. This proof uses Littleroot map data relocated to CPU address `0x12000000`.
- **Do not edit** `DevElectrum/grand-anthology` for either emulator or Emerald tests.
- User explicitly requires a **fast-forwardable XGBA**; never again distribute or recommend a build without turbo. Treat this as a required acceptance gate. The expected keyboard shortcut is **hold Tab to accelerate, release Tab to restore normal speed**.

## Why the first SDL build felt not fast-forwardable

- `src/platform/sdl/sdl-events.c` already handled Tab by toggling `context->impl->sync.audioWait`.
- Both renderer backends also forced VSync: `src/platform/sdl/sw-sdl2.c` used `SDL_RENDERER_PRESENTVSYNC` / SDL3 `SDL_SetRenderVSync(..., 1)`; `src/platform/sdl/gl-common.c` used `SDL_GL_SetSwapInterval(1)`. SDL VSync can cap the framerate even if core audio waiting is disabled. This was a plausible cause based on code inspection, not yet instrumentally measured on Nyx.
- `xgba/p1-fast-forward` removes renderer VSync in software/SDL2, SDL3 and OpenGL paths. SDL defaults retain audio pacing at normal speed. Tab is handled before GBA-key mapping and changes audioWait under the audio synchronization mutex.
- `tools/xgba_check_fast_forward.py` guards the SDL keybinding/VSync policy in CI; it is **static source verification only** and cannot establish actual turbo speed.

## Fast-forward acceptance gate

1. Build and run the SDL emulator from **`xgba/p1-fast-forward`**. Confirm the binary is from that branch, not the old `xgba/p0-validation-20261030` checkout or system-installed mGBA.
2. Open the existing `~/Projects/xgba-emerald-test/xgba-emerald-e0.gba`.
3. Compare elapsed in-game time on Nyx at normal speed and while holding Tab for equivalent wall-clock intervals. Expect an obvious acceleration, preferably over 2x; release Tab to immediately return to normal.
4. Check no audio deadlocks, input problems or hangs while pressing/releasing Tab repeatedly. Save, quit, reopen normally.
5. If turbo does not accelerate, investigate runtime renderer choice, swap-interval support, video synchronization and the audio producer; do not claim fixed based only on compiler and static contract pass.

## Build instructions on Pop!_OS

```bash
cd ~/Projects/xgba
git fetch origin
git switch xgba/p1-fast-forward
git pull --ff-only
cmake -S . -B build-xgba -G Ninja -DCMAKE_BUILD_TYPE=Debug -DM_CORE_GBA=ON -DM_CORE_GB=OFF -DBUILD_QT=OFF -DBUILD_SDL=ON -DSDL_VERSION=2 -DBUILD_HEADLESS=ON -DBUILD_SUITE=ON -DENABLE_SCRIPTING=OFF -DUSE_FFMPEG=OFF -DUSE_EDITLINE=OFF -DUSE_DISCORD_RPC=OFF
cmake --build build-xgba --parallel 8
./build-xgba/sdl/mgba ~/Projects/xgba-emerald-test/xgba-emerald-e0.gba
```

If the branch isn't local, use `git switch -c xgba/p1-fast-forward --track origin/xgba/p1-fast-forward`.

## What is not yet established

- Turbo **actual** acceleration, audio and UI performance on Nyx.
- Whether the expanded ROM can handle full 20 independently selectable Hoenn worlds and expanded save/PC storage.
- Emulator-side patches/cheats across legacy and expanded ROM aliases are incomplete. Avoid use in XGBA ROM experiment.
- Android build and gameplay are untested.

## Handoff maintenance

After each meaningful milestone, update this file with exact branch/commit, CI URL/status, tested results, unresolved bugs, the next actionable steps. Keep plans distinct from verified facts.

# XGBA living handoff for future Astras

Updated: 2026-10-30

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

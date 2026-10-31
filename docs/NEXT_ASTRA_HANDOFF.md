# XGBA living handoff for future Astras

## 2026-10-31: COMPANION E6 V2 SPEED RESULTS FROM USER (NO EMULATOR CHANGE)

After the game repo `xgba/e6-world-virtualization` optimized page-store v2 implementation, the user reports **~1 second PC page changes at 1× emulator speed** and **~1.5–2 seconds world changes at 1×**. Original v1 delays were **~6–7 seconds PC pages and ~3–4 seconds world changes at 5×**. These are different emulator speeds; don't invent an exact benchmark ratio. Performance is now considered responsive enough to continue E6 development, with no emulator optimization or source changes requested. Remaining gameplay acceptance item: v2 Page 8 Pokémon and world/inventory data after native save/full restart, which was not confirmed in this message. Existing old E5 and E6 saves and backups remain protected.


## 2026-10-31: Companion game E6 optimized PC page format implemented (NO EMULATOR CODE CHANGE)

The user authorized a single adversarially planned speed-optimization sprint after **6–7sec PC page switches** and **3–4sec world travel** at **5× fast-forward**. Game branch `DevElectrum/xgba-emerald-test@xgba/e6-world-virtualization` implements CRC32-protected independent A/B records for each 33,744-byte PC page, plus two tiny selected-page metadata records, without EWRAM growth or native flash change. File sidecar remains `0x380000..0xE1FFFF` (20 worlds × `0x88000`). Fast ROM CRC table, validated resident-page cache, skipped unchanged writes, and accelerated world-snapshot handling. All 0x0F memory accesses remain 8-bit.

**User's existing E6 v1 save remains separate and unchanged.** The new game ROM rejects old v1 sidecar banks until the user closes XGBA and performs a **create-only offline conversion** using `tools/xgba_e6_box_upgrade.py ORIGINAL.sav NEW.sav`. Converter checks old CRCs, exact payload preservation across all initialized pages, new A/B redundancy, and unchanged native/E5/E6 world/RTC and other bytes. Keep the original save separately backed up; never open a v2 save with an older v1 game ROM. No remote changes to user's local saves.

Companion game's automated migration/capacity/corruption tests passed https://github.com/DevElectrum/xgba-emerald-test/actions/runs/41192005262. C source through `e755c222` built green https://github.com/DevElectrum/xgba-emerald-test/actions/runs/41191853435; consult latest Actions after subsequent code before sending user tests. **Optimized gameplay latency and cold restart remain untested.** Game handoff/plan provide precise safe deployment and acceptance expectations.

**Emulator E6 code is unchanged.** Preserve `xgba/e6-large-save`, E5 stable branch, and every save backup. Mandatory adversarial E(i) engineering standard is active.


## 2026-10-31: USER-MANDATED ENGINEERING PROCESS FOR EVERY FUTURE E(i)

**Permanent instruction, also written into this repo's `AGENTS.md` and the companion game's `AGENTS.md`:** The user wants Regieleki-speed implementation **with front-loaded performance, error handling and adversarial planning**. They accept legitimate post-test optimization, but do NOT want to spend three successive gameplay waves finding architectural oversights in each E(i). A passing build or happy-path feature is not a finished milestone.

**Before coding:** define requirements and invariants; estimate realistic frequent/worst-case CPU, RAM, persistent-write, checksum and observed-latency costs at maximum capacity; compare alternatives; explicitly consider power-loss/torn writes, corruption, cache desynchronization, file bounds, format-version mismatch, migration and rollback; run an internal Architect-vs-Adversary design challenge. Set measurable success targets. **During implementation:** preserve E5 and populated E6 saves, use versioning and verified backups, implement recovery, and automate relevant boundary/full-capacity/corrupt-data tests. **Acceptance:** design gate → CI/error-path gate → one short user gameplay round with correctness and performance. Avoid re-testing accepted results. Significant genuine surprises may be fixed after testing, but predictable bottlenecks should be anticipated.

**Latest actual user tests in companion game:** E6 Pokémon PC **PAGE 8 / BOX 1 Zigzagoon survives page changes**, and World 3 Poké Balls **reappear correctly upon returning to World 3**. Both tests passed in gameplay; do not repeat them except to verify a materially new storage format. However performance is **unacceptable**: PC page changes take about **6–7 seconds even at 5× fast-forward**; world changes take about **3–4 seconds at 5×** after the PC expansion. This is the architectural oversight motivating optimization, not an emulator mapping failure proven by these timings.

**NEXT = PLANNING, NOT CODE:** Prepare an adversarially reviewed optimized page-store proposal: independent per-page records (two CRC-protected copies per page, ~33.8 KB each), avoid two ~270 KB whole-bank copies per switch, faster CRC checks, no redundant checkpoints, compatible safe migration of already populated existing E6 sidecar banks with explicit backup/readback/rollback, worst-case timing and corruption tests. Cross-check exact save offsets and conversion staging: preserving old data during migration is essential, not assumed. User asked to record rules only; **do not start the optimization implementation in response to this instruction**. Read companion game's handoff and design document before work.


## 2026-10-31: Eight-page PC bank now implemented in companion E6 game (NO emulator change)

- Companion game branch `DevElectrum/xgba-emerald-test@xgba/e6-world-virtualization` has **implemented 112-boxes/world streamed storage** (8 native pages ×14 boxes) with redundant per-world sidecar banks occupying file region 0x380000..0xE1FFFF, accessed using already implemented E6 extension address `0x0F360000`. New `src/xgba_e6_boxes.c` and `src/pokemon_storage_system.c` CHANGE PC PAGE UI.
- Game E6 ROM CI source build GREEN https://github.com/DevElectrum/xgba-emerald-test/actions/runs/41188529114 (code through `3a622364`); newer Python inspector and tests check CRC/offsets. **PC gameplay has NOT YET been user-tested.** Do not claim game success merely from emulator/ROM CI.
- Companion E6 inventory fix commit `a4990784` removes `SavePlayerBag()` stale load-cache overwrites during portal transitions and save checkpoints; leaves `LoadPlayerBag()` to refresh caches after restore. Game compilation success; gameplay inventory still needs minimal validation.
- **No new XGBA emulator changes needed for PC pages**, because existing E6 16 MiB mapping already supports all addresses. Continue emulator on `xgba/e6-large-save` if future bugs arise. Protect user's `xgba-emerald-e6.sav` with a timestamped copy before first live box test. Game handoff and `docs/XGBA_E6_PLAN.md` contain exact current commit/CI and test steps. User wants 2 commands (game update+build+backup, Qt launch) and fast iteration.

## 2026-10-31: GAMEPLAY ACCEPTANCE UPDATE FROM E6 GAME REPO

The user verified **independent party state for World 3 and World 20** in five screenshots: World 20 has only Torchic Lv6 6/22 HP, World 3 has Torchic Lv8 + Zigzagoon + two Poochyena, then caught another Zigzagoon; returning to World 20 left its party and HP unchanged; returning to World 3 retained all five Pokémon. No native save or emulator reboot took place during the sequence. Thus **in-session world party/capture isolation passes**. Do not call E6 cold-restart party persistence verified by this test. Game repo handoff commit `246a35bf`; code unchanged in emulator. E6 next: 8 streamed native 14-box PC pages per world (112 boxes, 2,240 total), keeping a single 33,744-byte PC in EWRAM and storing pages in the existing 16 MiB extension. See companion `docs/XGBA_E6_PLAN.md` for design. Preserve E5 and E6 save files; skip redundant E6 party testing.


## 2026-10-31: E6 16 MiB emulator MILESTONE GREEN, new branch active

- **Real development branch** `xgba/e6-large-save` created from accepted `xgba/e5-save-expansion`. E6 code exists: `include/mgba/internal/gba/memory.h`, `savedata.h`, `src/gba/savedata.c`, `memory.c`, `src/gba/test/core.c`; `.github/workflows/xgba-e6.yml` added. Baseline E5 kept untouched.
- ROM revision `0xE6` separately enables a **16 MiB E6 save envelope** only when the matching v2 CRC32 superblock at file offset 0x20000 or 0xFFF000 is valid; 0x0F maps file offset 0x20000 through 0xFFFFFF, and native Flash1M first 128 KiB continues normally at 0x0E. E5 revision 0xE5 still maps 1 MiB and runs its old test. The E6 test writes high 0x0F addresses, reads after emulator shutdown/reload, and tests backup-header / invalid-header fallback.
- **Verified GREEN core CI** (including E6 persistence test): https://github.com/DevElectrum/xgba/actions/runs/41182582688 . Code HEAD for first passing E6 build: `69050430`. CI workflow later refined only to avoid docs-triggered rebuilds (`bfd9e6d7`). Qt user runtime of E6 not yet tested.
- Companion game branch `xgba/e6-world-virtualization` now has `make e6`, Python copy-only save migration, 53,380-byte SaveBlock1/SaveBlock2/PokemonStorage world records, and an automatic world-portal state-switch prototype. Emulator interface and game C code must be tested **together**; see companion `docs/XGBA_E6_PLAN.md`. The game native compiler CI was still running at this checkpoint.
- **Next:** inspect game E6 CI and fix failures; once green, guide the user through a copy-only E5→E6 save, two-branch update/build, Qt E6 ROM launch, and focused World 3/20 smoke test. E6 PC expansion to 100 boxes/world and playable cloned story events remain pending. Do not declare those done. Never overwrite accepted E5 `.sav`.


## CURRENT NEXT ASTRA CHECKPOINT: 2026-10-31 (E6 large-save strategy; no E6 code yet)

The user emphasized this is a **passion project**, not a business release. Prefer large ambitious implementation leaps, quick iteration and focused smoke tests rather than exhaustive QA. Motto: **Regieleki, speed is god**. A critical process requirement is to **update the local-in-repository handoff and agent instructions periodically**, especially at every milestone, blocker and architectural pivot, so another Astra can take over a long-running project without reconstructing context.

**Read first:** this file, local [`AGENTS.md`](../AGENTS.md), and companion game repo `DevElectrum/xgba-emerald-test` docs `AGENTS.md`, `docs/NEXT_ASTRA_HANDOFF.md` and `docs/XGBA_E6_PLAN.md` (on game branch `xgba/e5-expanded-save` until an E6 feature branch is created). The full E6 architectural plan is in the companion game's E6 document.

**Verified baseline:** emulator branch `xgba/e5-save-expansion` already supports a 1 MiB versioned E5 save and a 896 KiB persistent 0x0F mapped extension. The user **personally confirmed** the game's World 3 and World 20 E5 snapshots survived world switches, normal game save and full emulator exit/restart. DO NOT require user to redo accepted E4/E5 tests; DO NOT claim the 1 MiB format stores full independent world states or huge Pokémon boxes.

**New E6 objective (approved as direction, unimplemented):** EXPERIMENTAL expansion to a versioned approximately **16 MiB save envelope** on a *new feature branch* (proposed `xgba/e6-large-save` from accepted E5; not yet created). Companion game aims to virtualize all 20 Hoenn worlds' actual gameplay state, revive cloned NPCs/events/story, and stretch toward ~100 PC boxes per world. The emulator change should preserve original 128 KiB native Flash1M behavior, support an explicit E5→E6 save-copy migration, and audit 0x0F address boundary, save dirty/flush, RTC, file size and opt-in marker/versioning. Do not silently open E5 saves as E6. Respect EWRAM limits on the game side. This is a high-risk prototype, so isolate in experiment branches and protect known-good saves.

**Current repo changes:** created `AGENTS.md` on E5 emulator branch (commit `1a66d9a0`); companion repo added E6 plan `8099a85a`, updated game `AGENTS.md` `060bee38` and game handoff `7af0cc93`. All documentation-only, no E6 runtime implementation or E6 user testing.

**Immediate next steps:** inspect both current branches/CI, create E6 experimental branches when coding starts, implement emulator large-save plumbing and game format/migration together, then world virtualization and PC growth. User's preferred local UX is 3 commands maximum when practical: update/build; optional copy/migrate save; Qt `mgba-qt` ROM launch. **GitHub commits do not appear on Nyx until user runs `git pull`.** Never modify `DevElectrum/grand-anthology` or `master` for the experiment.

**Handoff contract:** after every meaningful code change, CI result, user test, blocker, or revised plan, update/commit this handoff and agent file as needed, plus companion notes if interfaces change. Include branch, exact commit SHA, changed files, tests/CI status and URLs, verified vs proposed, blocking issue and next concrete step. Commit before conversation context is exhausted.


## 2026-10-31: E5 LIVE GAMEPLAY PERSISTENCE ACCEPTED

**USER CONFIRMED BOTH PHASES PASSED.** With the E5 Qt XGBA build and the separately initialized `xgba-emerald-e5.sav`, the user visited World 20 and World 3. In each world, the E5 writer successfully recorded the existing 16-byte E4 prototype event state and the E5 auditor reported a match. Returning between worlds preserved each independent E5 snapshot. After a normal in-game save, complete emulator shutdown, and reload, both world's E5 auditors again reported matches. This is direct user verification of guest 0x0F writes surviving the emulator's save-file cycle.

The final reachable E5 NPC locations in cloned Littleroot are **writer (14,15)** and **auditor (16,15)**. The original (20,12)/(22,12) placements were obstructed by trees and have been fixed in the generator. The user confirmed the replacement NPCs are reachable.

**Accepted scope:** E5 opt-in 1 MiB save bridge and independent 16-byte snapshot read/write/restart proof for Worlds 3/20. **Not accepted:** full world-local trainer/story data, Pokémon PC box expansion, maximal 20,448-byte record stress tests, game-level torn-write recovery, all-world coverage, and end-to-end RTC compatibility. The original E4 save must remain backed up. Do not ask the user to repeat this already successful acceptance test.

User workflow preference: **three commands whenever practical**: (1) update repositories/build, (2) create or copy save only if applicable, (3) run the Qt `mgba-qt` with the new ROM. For ROM-only patches, give two commands: update/rebuild, launch.

Next: choose the next design milestone. The 20 KiB E5 per-world payload cannot accommodate a vanilla 33,744-byte `PokemonStorage` wholesale; do not claim full Pokémon boxes are supported. Plan larger or differently packed world/PC storage separately. Keep `DevElectrum/grand-anthology` and both `master` branches untouched.

- **2026-10-31 E5 integration gate update:** emulator CI https://github.com/DevElectrum/xgba/actions/runs/41172551382 passed, including the new E5 persisted-save/backup-header/fail-closed regression. Companion game branch native and tagged ROM CI also passed: https://github.com/DevElectrum/xgba-emerald-test/actions/runs/41172674059 and https://github.com/DevElectrum/xgba-emerald-test/actions/runs/41172674073. The next gate is user gameplay using an independently initialized 1 MiB E5 save, NOT modifying the accepted E4 original.

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

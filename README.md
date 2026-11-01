# XGBA

XGBA is an open-source Game Boy Advance emulator based on [mGBA](https://github.com/mgba-emu/mgba), developed as an extended-cartridge fork by [DevElectrum](https://github.com/DevElectrum).

It supports ordinary GBA games and opt-in **XGBA256** cartridges, with images up to **256 MiB** accessible at guest CPU addresses `0x10000000..0x1FFFFFFF`. Optional E5 (1 MiB) and E6 (16 MiB) expanded saves require a matching cartridge revision and pre-initialized save headers. Native cartridge save behavior is unchanged.

## Application commands

- `xgba`: installed desktop GUI launcher; passes arguments to `xgba-qt`.
- `xgba-qt`: full Qt desktop GUI, and standalone development executable.
- `xgba-sdl`: optional SDL frontend and its CMake target.
- `xgba-headless`: optional headless tool.

After installation, launch the GUI with `xgba` or `xgba /path/to/game.gba`. The SDL build is available as `xgba-sdl`. Use `--help` for options. The purple XGBA handheld identity is integrated across the empty-screen artwork, About view, app/window icons, desktop launcher, and native packaging. XGBA retains upstream mGBA copyright and MPL-2.0 notices. Localized mGBA translation catalogs are not loaded until they can be rebranded accurately.

## Build on Linux

On Ubuntu/Pop!_OS install CMake, Ninja, C/C++ development tools, Qt 6, SDL2, zlib and Python 3:

```sh
sudo apt-get install build-essential cmake ninja-build pkg-config zlib1g-dev libsdl2-dev qt6-base-dev qt6-multimedia-dev libqt6opengl6-dev qt6-l10n-tools
cmake -S . -B build-xgba -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_QT=ON -DBUILD_SDL=ON -DFORCE_QT_VERSION=6 -DUSE_FFMPEG=OFF -DENABLE_SCRIPTING=OFF -DUSE_DISCORD_RPC=OFF
cmake --build build-xgba --target xgba-qt xgba-sdl --parallel 2
./build-xgba/qt/xgba-qt
```

See [upstream build documentation](https://github.com/mgba-emu/mgba#building) for other platforms. No copyrighted ROM or BIOS is bundled.

## Project branches and compatibility

- `master`: original upstream-facing baseline, retained.
- `xgba/e7-accepted-20261031`: user-accepted 256 MiB ROM and experimental expanded-save support.
- `release/xgba-v0.1.0`: application identity and packaging work.

Experimental extended saves can require a separate, version-specific conversion step. **Always keep a backup of your original save.** XGBA does not check mGBA's update endpoint and has no automatic update service pending its own authenticated release feed.

## Credits and license

XGBA is derived from [mGBA](https://github.com/mgba-emu/mgba) by Jeffrey Pfau and contributors. Original copyright and [MPL 2.0](LICENSE) notices remain intact. XGBA modifications © 2026 DevElectrum. XGBA is not affiliated with Nintendo or the upstream mGBA team.

## Install locally on Pop!_OS / Ubuntu

From a fresh separate checkout (not your preserved E7 development worktree):

```sh
cmake -S . -B build-release -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local -DBUILD_QT=ON -DBUILD_SDL=ON -DSDL_VERSION=2 -DFORCE_QT_VERSION=6 -DENABLE_SCRIPTING=OFF -DUSE_FFMPEG=OFF -DUSE_DISCORD_RPC=OFF
cmake --build build-release --parallel 4
sudo cmake --install build-release
sudo ldconfig
hash -r
xgba
```

The system install provides `xgba` (GUI launcher), `xgba-qt`, and `xgba-sdl`, plus an application-menu desktop entry. Save files and configuration stay in your home directory, separate from upstream mGBA. `/usr/local/bin` must appear in `PATH`. A local install manifest is written to `build-release/install_manifest.txt` for auditing the installed paths; do not blindly delete shared libraries that another application may use.

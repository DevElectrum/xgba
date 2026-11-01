# XGBA

XGBA is an open-source Game Boy Advance emulator based on [mGBA](https://github.com/mgba-emu/mgba), developed as an extended-cartridge fork by [DevElectrum](https://github.com/DevElectrum).

It supports ordinary GBA games and opt-in **XGBA256** cartridges, with images up to **256 MiB** accessible at guest CPU addresses `0x10000000..0x1FFFFFFF`. Optional E5 (1 MiB) and E6 (16 MiB) expanded saves require a matching cartridge revision and pre-initialized save headers. Native cartridge save behavior is unchanged.

## Application commands

- `xgba-qt`: Qt desktop GUI, recommended.
- `xgba`: SDL frontend and terminal command.
- `xgba-headless`: optional headless tool.
- `xgba-sdl`: CMake target name for the SDL executable; the installed command is `xgba`.

Launch with `xgba-qt /path/to/game.gba` or `xgba /path/to/game.gba`. Use `--help` for options. The original mGBA logo artwork is temporarily retained, but executable identity, desktop integration and configuration paths are independent. Localized mGBA translation catalogs are not loaded until they can be rebranded accurately.

## Build on Linux

On Ubuntu/Pop!_OS install CMake, Ninja, C/C++ development tools, Qt 6, SDL2 and zlib:

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

# RAP-Vita – development notes

- Toolchain: VitaSDK (x86_64 Linux build) + `vdpm sdl2` (`sdl2.tar.xz` extracted into `$VITASDK/arm-vita-eabi`).
- Build: `cmake -S . -B build-vita -DVITA_BUILD=ON -DCMAKE_BUILD_TYPE=Release && cmake --build build-vita`
- Version: `RAPVITA_VER` / `VITA_VERSION` in `CMakeLists.txt` (VPK name: `Rap-Vita_v<version>.vpk`).
- Title ID: `RAPT00001`.

## Vita-specific files
| File | Purpose |
|---|---|
| `src/vita_menu.cpp` | SELECT menu (settings, button mapping, screenshot), on-screen keyboard, EN/DE texts, settings in `SETUP.INI` (`[Vita]`, `[Keys0..2]`) |
| `src/vita_debug.cpp` | optional debug log `ux0:data/Raptor/log.txt` (heartbeat every 2 s, function trace) |
| `src/vita_compat.h` | force-included header: `access()` via `stat()`, shared declarations, `VTRACE` |
| `src/i_video.cpp` | rendering (screen formats, rotation), touch mapping, screenshots |
| `src/joyapi.cpp` | pointer speed, right-stick pointer, per-orientation steering stick, D-pad rotation |
| `src/input.cpp` | D-pad sensitivity, in-game touch steering |
| `src/windows.cpp` | on-screen keyboard in the pilot registration window |
| `vita/sce_sys/` | LiveArea artwork (original, 8-bit PNG) |

Search for `__vita__` to find all Vita changes in the upstream code.

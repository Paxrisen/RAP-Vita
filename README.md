# RAP-Vita

**Raptor: Call of the Shadows – native PlayStation Vita port**

RAP-Vita is a native PS Vita port of *Raptor: Call of the Shadows*. It is based on the open-source,
reverse-engineered source port **[skynettx/raptor](https://github.com/skynettx/raptor)**
(itself based on the reverse-engineered codebase by nukeykt), licensed under the GPL-2.0.

> **No game data is included.** You need your own copy of Raptor (e.g. the GOG version, v1.2 or later)
> and copy its data files to your Vita.

## Features

- Runs natively on the PS Vita (no emulator) at the original game speed
- Full gamepad support, menus usable with a pointer (both analog sticks) or the **front touch screen**
- Built-in on-screen keyboard for pilot name and callsign
- **RAP-Vita menu** (press **SELECT** anytime) in **English and German**:
  - Settings: language, pointer speed, right stick as pointer, front touch on/off,
    touch controls in game (off / steer / steer + fire), D-pad sensitivity (1–5),
    screen format (4:3 / wide / fullscreen), orientation (landscape / portrait R / portrait L),
    portrait only during levels, debug log
  - Button mapping: separate layouts for **landscape, portrait R and portrait L**
    (fire, special weapon, change weapon, mega bomb, steering stick), restore defaults
  - Screenshots (press **R** in the RAP-Vita menu)
- **Two players on one Vita** (SELECT menu, available once a game is loaded):
  - **Twin Mode (landscape):** two ships on one screen – red uses the left side, blue the right side
  - **Co-op portrait (split screen, L/R):** the screen is split in two, each player sits at one end of the Vita
  - Shields, weapons, ammo and money are shared; red and blue triangles mark the ships
- Rear touch is disabled

## Installation

1. Install `Rap-Vita_v1.1.vpk` with VitaShell.
2. Create the folder `ux0:data/Raptor/`.
3. Copy `FILE0000.GLB` … `FILE0004.GLB` from the `data` folder of your Raptor installation (e.g. GOG)
   to `ux0:data/Raptor/` (file names are not case-sensitive; `FILE0005/0006` optional).
4. Start **RAP-Vita**.

Save games, `SETUP.INI` and screenshots (`screenshots/`) are stored in `ux0:data/Raptor/`.

## Default controls

| Action | Landscape | Portrait R | Portrait L |
|---|---|---|---|
| Fly | Left stick / D-pad | Left stick | Right stick |
| Fire | Cross | R | L |
| Special weapon | Circle | L | R |
| Change weapon | Square | D-pad left | Circle |
| Mega bomb | Triangle | D-pad right | Square |
| Pause | Start | Start | Start |
| RAP-Vita menu | Select | Select | Select |

In menus: move the pointer with either stick or tap the screen, **Cross** = click.
All buttons can be remapped in **SELECT → Button mapping**.

## Two players

Enable it in the **SELECT** menu while you are in the hangar (a game must be loaded). Player 1 (red) leads in
the hangar and shop; both players share shields, weapons, ammo and money. It is active during levels.

| | Red (left side) | Blue (right side) |
|---|---|---|
| Fly | Left stick | Right stick |
| Fire | L | R / Cross / Square |
| Change weapon | D-pad left/right | Circle |
| Mega bomb | D-pad up | Triangle |

**Co-op portrait (split screen):** each half shows the whole game, rotated towards the player at that end of
the Vita; sticks and D-pad are rotated to each player's view (push away from you = fly up).
**L** = red sits at the left end (left controls), **R** = red sits at the right end (right controls).

## Building

Requirements: [VitaSDK](https://vitasdk.org) with the `sdl2` package (`vdpm sdl2`).

```sh
export VITASDK=/usr/local/vitasdk
./build_vita.sh
# -> build-vita/Rap-Vita_v1.1.vpk
```

A GitHub Actions workflow (`.github/workflows/build.yml`) builds the VPK automatically on every push.
See [DEVELOPMENT.md](DEVELOPMENT.md) for an overview of the Vita-specific code.

## Credits

- **skynettx** – [Raptor source port](https://github.com/skynettx/raptor)
- **nukeykt** – reverse engineering of the original game
- **Scott Host / Mountain King Studios** – original game *Raptor: Call of the Shadows*

## License

GPL-2.0 – see [LICENSE](LICENSE). The original upstream README is kept as [README_skynettx.md](README_skynettx.md).

*Raptor: Call of the Shadows* is a trademark of its respective owners. This project is not affiliated
with or endorsed by them. The LiveArea artwork was created for this project and contains no original game assets.

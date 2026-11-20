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
3. Copy `FILE0000.GLB` … `FILE0006.GLB` from the `data` folder of your Raptor installation (e.g. GOG 2010 version [2015 steam version dont work])
   to `ux0:data/Raptor/`
4. Start **RAP-Vita**.

Save games, `SETUP.INI` and screenshots (`screenshots/`) are stored in `ux0:data/Raptor/`.

## Troubleshooting

**"Could not load SoundFont.sf2" error at startup**

This happens when a `SETUP.INI` from the PC version (e.g. GOG) was copied to the Vita. The PC version uses
General MIDI music, which needs a `SoundFont.sf2` file that the Vita doesn't have.

Fix: open `ux0:data/Raptor/SETUP.INI` and make sure the `[Music]` section contains this line:

```ini
[Music]
CardType=5
```

`CardType=5` uses the built-in AdLib/OPL music, just like the original DOS sound. You can also simply delete
`SETUP.INI`; RAP-Vita then creates a fresh one with the correct settings.
Since **v1.2.1**, RAP-Vita switches to AdLib music automatically when no SoundFont is found.

**The game doesn't start / crashes right away**

No plugins are required. If you use many plugins, disable the ones listed under `*ALL` in your
`ux0:tai/config.txt` (or `ur0:tai/config.txt`), reboot and try again.

## PS Vita

PS Vita or PS TV on firmware 3.60–3.74 with HENkaku / Ensō (or h-encore)
VitaShell to install the VPK and copy files
kubridge 0.3.1 or newer in ur0:tai/config.txt under *KERNEL (required)
Recommended: vita-turbo-storage

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

## Community & Support
Want to share feedback, report bugs, suggest new features, or just chat with other users? Join our community!

👉 **[Join the GameNexum Discord Server](https://discord.gg/xkSFYMCgHH)**

## Credits

- **skynettx** – [Raptor source port](https://github.com/skynettx/raptor)
- **nukeykt** – reverse engineering of the original game
- **Scott Host / Mountain King Studios** – original game *Raptor: Call of the Shadows*

## License

GPL-2.0 – see [LICENSE](LICENSE). The original upstream README is kept as [README_skynettx.md](README_skynettx.md).

*Raptor: Call of the Shadows* is a trademark of its respective owners. This project is not affiliated
with or endorsed by them. The LiveArea artwork was created for this project and contains no original game assets.

## 🤖 Acknowledgments & Credits

* **Porting & Code Generation:** This port was fully developed and generated with the assistance of **Claude Code** (Anthropic's AI coding assistant).
* **Target Platform:** Sony PlayStation Vita / PS TV.
* **Special Thanks:** The PS Vita homebrew community for their open-source toolchains and documentation.

# Changelog

> RAP-Vita is made with AI assistance (Claude by Anthropic). Ideas and hardware testing: Paxrisen.

## v1.3
**Co-op on two Vitas**
- Ad-hoc (direct) or Wi-Fi (same network) co-op, in sync on both Vitas; the host picks the mission, the guest waits in the hangar
- Pause and quit work for both players (hold Select for 1.5 s to leave)

**Co-op difficulties: Easy, Fair, Hard, Expert**
- Fair: tougher enemies that fire more often
- Hard: own energy and super shield for each player, revive after 12 kills (for both players), game over only when both are down
- Expert: like Hard, plus 3× enemy health, more fire, faster aircraft and partly cloaked enemies
- Fixed shield bars in the bottom corners, low-shield warning for both players
- Each player picks their own special weapon; P1/P2 labels in the HUD; mega bombs move below the super-shield icons

**Menus**
- New Co-op modes page; hangar button row: Vita menu, Co-op, 2 players (quick toggle), Daily, Medals
- Option to turn off the triangles under the ships

**Other**
- Custom music: WAV files in `ux0:data/Raptor/music/` replace the matching tracks
- Total play time counter in the statistics
- 2 new co-op medals (28 in total)
- Fix: player 2 had no muzzle flash
- Fix: enemy shadows flickered with the "Fast enemies" modifier

## v1.2.2
- Fix: black screenshots
- "Fast enemies" only affects aircraft
- Version number centred in the main menu

## v1.2.1
- Fix: "Could not load SoundFont.sf2" no longer quits the game (falls back to AdLib)
- Fix: menu input, button-mapping labels, PC `SETUP.INI` forced to gamepad
- New: 2-player button mapping, D-pad selects menu items, stick/D-pad speed and dead zone

## v1.2
**Menus**
- VITA MENU button in the main menu and hangar; DAILY CHALLENGE button in the hangar
- SELECT opens the menu only during missions; L/R work as mouse buttons in menus
- Abort mission (back to hangar) from the RAP-Vita menu
- Fast scrolling (hold Up/Down), L/R jumps 10 entries, draggable scrollbars, more reliable button input

**Medals & unlockables**
- 26 medals with pop-ups and progress, pilot helmets for resetting at 100 %, an unlockables page

**Extras**
- Ship paints and weapon colours for both players, with list pages and live preview
- Screen filters: CRT, LCD, Greyscale, Green monitor, Sepia, Night vision
- 17 pilot portraits, including the shop owner from the game data
- Boss hangar and enemy hangar: recolour bosses and enemies, use their paint, best boss times
- 9 modifiers: Hardcore, Ghost mode, Turbo credits, Endless bombs, Mirrored steering, Glass cannon, Main gun only, Fast enemies, Turbo speed
- Daily challenge, records and sector ranks (Bronze to Diamond), mission report after every level
- Statistics, encyclopedia, jukebox (16 tracks), photo mode

**Co-op**
- Markers drawn in true colour (no blinking); colour selectable per player, or off

**Pilot creation**
- Name, callsign, portrait and difficulty (original names) on the keyboard screen, then straight to the hangar
- Training hint ("only 4 of 9 levels"); no more jumping back to the register window

**Fixes**
- Crash when choosing certain pilot portraits (unaligned sprite data)
- Graphics glitches and missing shield bars after closing the menu
- Debug log is always off at start

## v1.1
- New: **Twin Mode (landscape)** – two ships on one screen, red = left side controls, blue = right side controls
- New: **Co-op portrait (split screen)** with L/R seating – each player sits at one end of the Vita
- Shared shields, weapons, ammo and money; red/blue marker triangles under the ships
- Two-player options only appear in the SELECT menu while a game is loaded
- Fix: SELECT menu no longer stays visible in the main menu after closing it

## v1.0
- First public release of RAP-Vita (native PS Vita port based on skynettx/raptor)
- Gamepad, pointer (both sticks) and front-touch support in menus
- On-screen keyboard for pilot name and callsign
- RAP-Vita menu (SELECT) in English and German: settings, button mapping, screenshots
- Touch controls in game (steer / steer + fire), D-pad sensitivity (1–5)
- Screen formats 4:3 / wide / fullscreen
- Portrait mode (R / L), optionally only during levels, with its own button layout per orientation
- Screenshots saved to `ux0:data/Raptor/screenshots/`, optional debug log
- New original LiveArea artwork


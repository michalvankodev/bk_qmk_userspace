# AGENTS.md

QMK **userspace** repo for Bastard Keyboards keymaps (fork of `bastardkb/qmk_userspace`, upstream remote `upstream` is configured). Contains only keymaps, build config, and vendored QMK modules — firmware lives in an external clone of **`bastardkb/bastardkb-qmk`** (the fork, *not* mainline qmk_firmware). Compiled with the QMK CLI; C keymap code, make, GitHub Actions.

## Project Overview

- Personal keymap: `bastardkb/charybdis/3x5` with keymap name `michalvanko` — an 11-layer layout (QWERTY + Colemak base via Q+P / Q+SCLN combos, Miryoku-inspired function/nav/media/numeral/symbols/pointer, home-row GACS mods, 3 gamepad layers). VIA + combos enabled.
- **Build target is `bastardkb/charybdis/3x5/splinktegrated_rev1`** (Michal's board is a socketed Splinky RP2040, USB PID `0x1832` — verify with `lsusb`). The bare `bastardkb/charybdis/3x5` name no longer resolves in `bastardkb-qmk@main`; `splinktegrated_rev1` is the official continuation (same keyboard_name/PID). `qmk.json` is the source of truth; `ogbuild_targets` is a stale reference list, `build_targets` is what CI/`qmk compile` builds.
- The `vendor` keymaps (all boards) are upstream defaults — don't edit; the `michalvanko` keymap was copied from `vendor` and diverged.

## Architecture

```
qmk.json                  # build_targets (what CI builds) — michalvanko only
keyboards/bastardkb/charybdis/3x5/keymaps/michalvanko/
  keymap.c  config.h  rules.mk  keymap.json
modules/bastardkb/        # VENDORED copy of Bastardkb/qmk_modules (NOT a submodule)
  argos/                  # + one local patch: layer cap 8→16 (see below)
  bk_pointing_device/     # pointer modes: sniping/drag-scroll/DPI/INVY, auto-mouse
  bk_led_matrix/ lcd/
users/  layouts/          # empty placeholders (.keep)
```

- A keymap dir is the unit of work: `keymap.c` (layers), `config.h` (feature defines), `rules.mk` (`VIA_ENABLE`, `COMBO_ENABLE`), **`keymap.json` (enables QMK community modules)** — currently `bastardkb/bk_pointing_device` + `bastardkb/argos`.
- In `keymap.c`: layers are an enum `charybdis_keymap_layers`; each layer is a `LAYOUT_LAYER_*` macro (3×10 grid + 5 thumb keys) composed via `HOME_ROW_MOD_GACS()` / `POINTER_MOD()` / `LAYOUT_wrapper()` into `keymaps[]`. Layout macros sit between `// clang-format off/on`.
- **Pointing features live in the `bk_pointing_device` module** — keymap.c has NO custom pointing callbacks. `INVY` on the media layer toggles inverted drag-scroll (natural scrolling), persisted to EEPROM. Old `CHARYBDIS_*` keymap APIs are gone.
- **⚠ Vendored module patch**: `modules/bastardkb/argos/post_config.h` hardcodes `DYNAMIC_KEYMAP_LAYER_COUNT 8` upstream; locally patched to 16 so the 11-layer keymap builds. When BastardKB lifts the cap, re-submodule: replace `modules/bastardkb` with `git submodule add https://github.com/Bastardkb/qmk_modules.git modules/bastardkb` and drop the patch.

## Code Conventions

- `.clang-format`: Google-based, 4-space indent, no tabs, align consecutive assignments/declarations, `ColumnLimit: 1000` (effectively no wrapping), includes never sorted.
- `.editorconfig`: UTF-8, trim trailing whitespace, final newline; tabs for `Makefile`/`*.mk`, LF for `*.mk`/`*.sh`.
- Preprocessor directives indented after the hash (`#    define`); headers use `#pragma once`; GPLv2 headers on vendor-derived C files.
- **Current QMK keycode names** (old aliases removed): `RM_TOGG`/`RM_NEXT`/`RM_PREV` (RGB matrix), `MS_BTN1`/`MS_BTN2`/`MS_BTN3` (mouse buttons). Module keycodes: `DRGSCRL`, `SNIPING`, `DPI_MOD`, `S_D_MOD`, `INVY`, `PZOOM`, `PVOLUME`, `PTABS`, `PHIST`, `CURSOR`, `PBRIGHT`.
- `DYNAMIC_KEYMAP_LAYER_COUNT` must be ≥ the enum layer count; make it unconditional in keymap `config.h` (argos' post_config overrides later unless vendored-patched).

## Setup Requirements

- QMK CLI (`python3 -m pip install --user qmk`, v1.2.0+) — after Python/nvm upgrades the `~/.local/bin/qmk` shim can lose `qmk_cli`; reinstall.
- **Firmware**: `~/Projects/bastardkb-qmk` (clone of `bastardkb/bastardkb-qmk@main`, submodules inited via `qmk git-submodule`). Configured via `qmk config user.qmk_home=~/Projects/bastardkb-qmk` and `user.overlay_dir=<this repo>`. Mainline `~/Projects/qmk_firmware` (updated, on master) is reference-only — it does **not** ship the Splinky/splinktegrated charybdis variants; old local patches (minidox EE_HANDS, miryoku QUOT/SCLN) are preserved in its `stash@{0}`.
- Toolchains: `arm-none-eabi-gcc` (RP2040) and `avr-gcc` both installed.
- My interactive shell is **fish** — present shell commands in fish syntax.

### Build / flash

```fish
qmk compile                                # builds everything in qmk.json build_targets
qmk compile -kb bastardkb/charybdis/3x5/splinktegrated_rev1 -km michalvanko
qmk flash -kb bastardkb/charybdis/3x5/splinktegrated_rev1 -km michalvanko   # BOOTSEL/double-tap reset to enter bootloader
```

## Important Files

- `qmk.json` — build targets; new keymaps must be registered here or CI won't build them.
- `.github/workflows/build_binaries.yaml` — on push: builds via `qmk/.github` reusable workflow against `bastardkb/bastardkb-qmk@main`, publishes UF2s as a release artifact (`contents: write`).
- `keymaps/michalvanko/config.h` — `QUICK_TAP_TERM 0` (done — flashed & verified): a tap-hold key re-pressed right after a tap can trigger the hold again, so the numeral layer re-engages after a backspace tap.
- `keymaps/michalvanko/keymap.c` — pointer behavior: `keyboard_post_init_user` forces the module's auto-precision (sniping) on every boot, active while `AUTO_MOUSE_DEFAULT_LAYER` (= LAYER_POINTER) is held; `INVY` on the media layer toggles Y-axis inversion of the current pointer mode (persisted). Trackball runtime config (per-mode DPI/invert) lives on the device EEPROM — tuned via Argos.
- Local Argos setup (done): `/etc/udev/rules.d/51-bastardkb.rules` grants HID access for VID a8f8; persistent Chromium profile at `~/.agent-browser-profiles/argos` keeps the WebHID grant. Current device state: drag-scroll Invert Y on (natural scrolling), Normal DPI 700, Precision 500.
- `.clang-format`, `.editorconfig`, `.clangd`; `.gitignore` excludes firmware outputs (`*.hex`, `*.bin`, `*.uf2` — build products land in `.build/` and repo root on compile).
- `LICENSE` — GPLv2.

# AGENTS.md

QMK **userspace** repo for Bastard Keyboards keymaps (fork of `bastardkb/qmk_userspace`). It contains only keymaps and build config — the firmware itself lives in an external `qmk_firmware` checkout (CI uses the `bastardkb/bastardkb-qmk` fork). Compiled with the QMK CLI; C keymap code, make, GitHub Actions.

## Project Overview

- Personal keymap: `bastardkb/charybdis/3x5` (Charybdis Nano) with keymap name `michalvanko`. This is the only entry in `qmk.json` `build_targets` and the only actively edited code.
- The `vendor` keymaps (all BastardKB boards: charybdis 3x5/3x6/4x6, dilemma, scylla, skeletyl, tbkmini) are upstream defaults kept for reference/CI (`ogbuild_targets`). Don't edit them; the `michalvanko` keymap was copied from `vendor` and diverged.
- Personal layout: QWERTY + Colemak base layers (switched via Q+P / Q+Semicolon combos), Miryoku-inspired layers (function/navigation/media/numeral/symbols/pointer), home-row GACS mods, plus custom gamepad layers. VIA + combos enabled.

## Architecture

```
qmk.json                  # userspace_version + build_targets (what `qmk compile` and CI build)
Makefile                  # proxies make to the qmk_firmware root (from `qmk config user.qmk_home`)
keyboards/bastardkb/<board>/keymaps/<name>/
  keymap.c  config.h  rules.mk  (readme.md in vendor keymaps)
users/  layouts/          # empty placeholders (.keep) — not used
.devcontainer/            # ghcr.io/qmk/qmk_cli container + setup.sh (installs qmk, clones firmware)
```

- A keymap dir is the unit of work: `keymap.c` (layers/keycodes), `config.h` (feature defines), `rules.mk` (feature toggles like `VIA_ENABLE = yes`, `COMBO_ENABLE = yes`).
- In `keymap.c`: layers are an enum `charybdis_keymap_layers`; each layer is a `LAYOUT_LAYER_*` macro (3×10 grid + 5 thumb keys); macros compose via `HOME_ROW_MOD_GACS()` and `POINTER_MOD()` wrappers into `LAYOUT_wrapper()` inside `const uint16_t PROGMEM keymaps[]`. Layout macros sit between `// clang-format off/on`.
- Pointing-device code (auto pointer layer, auto-sniping) is guarded by `#ifdef POINTING_DEVICE_ENABLE` / `CHARYBDIS_*` defines.

## Code Conventions

- `.clang-format`: Google-based, 4-space indent, no tabs, align consecutive assignments/declarations, `ColumnLimit: 1000` (effectively no wrapping), includes never sorted. Run `clang-format` on C changes.
- `.editorconfig`: UTF-8, trim trailing whitespace, final newline; tabs for `Makefile`/`*.mk`, LF for `*.mk`/`*.sh`.
- Preprocessor directives indented after the hash (`#    define`); headers use `#pragma once`.
- C files carry the GPLv2 header (copyright Charly Delay on vendor-derived code — keep it when deriving).
- Match the existing keymap style: `KC_*` keycodes, `LT()`/`TG()`/`DF()` shorthands as `#define`s near the top, doxygen-ish `/** \brief */` comments on layer macros.

## Setup Requirements

- QMK CLI (`pip install qmk`) with firmware location configured: `qmk config user.qmk_home=~/Projects/qmk_firmware` (local clone exists there). **Verify `qmk --version` works before building** — after Python/nvm upgrades the CLI at `~/.local/bin/qmk` can lose its `qmk_cli` module; reinstall with `python3 -m pip install qmk`.
- Alternative: open the repo in the devcontainer (`.devcontainer/`), which handles everything via `setup.sh`.
- No env vars or `.env` files. My interactive shell is **fish** — present shell commands in fish syntax.

### Build / flash

```fish
qmk compile                                # builds everything in qmk.json build_targets
qmk compile -kb bastardkb/charybdis/3x5 -km michalvanko
qmk flash -kb bastardkb/charybdis/3x5 -km michalvanko   # needs keyboard in bootloader mode
make                                       # alternative: proxied to qmk_firmware (requires user.qmk_home)
```

## Important Files

- `qmk.json` — **source of truth for build targets**. Add a new keymap here (as `["keyboard", "keymap"]`) or CI/`qmk compile` won't build it. `ogbuild_targets` are the upstream vendor builds.
- `.github/workflows/build_binaries.yaml` — on every push: builds via `qmk/.github` reusable workflow against `bastardkb/bastardkb-qmk@bkb-master`, then publishes firmware binaries as artifacts (has `contents: write`).
- `.clang-format`, `.editorconfig`, `.clangd` — C style; `.clangd` is tuned for clangd on QMK's embedded compiler flags.
- `.gitignore` — firmware outputs (`*.hex`, `*.bin`, `*.uf2`) are never committed.
- `LICENSE` — GPLv2.

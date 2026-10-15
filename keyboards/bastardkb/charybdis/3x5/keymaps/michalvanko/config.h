/**
 * Copyright 2021 Charly Delay <charly@codesink.dev> (@0xcharly)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#pragma once

/* VIA configuration: 11 layers in this keymap. */
#define DYNAMIC_KEYMAP_LAYER_COUNT 11

#ifndef __arm__
/* Disable unused features. */
#    define NO_ACTION_ONESHOT
#endif // __arm__

/* Charybdis-specific features.
 *
 * Pointer modes (sniping, drag-scroll, DPI, auto-mouse layer) are handled by
 * the `bastardkb/bk_pointing_device` module — see keymap.json.
 */

/* If the auto-mouse layer ever activates, make it the pointer layer (LAYER_POINTER = 5). */
#ifdef AUTO_MOUSE_DEFAULT_LAYER
#    undef AUTO_MOUSE_DEFAULT_LAYER
#endif
#define AUTO_MOUSE_DEFAULT_LAYER 5

/* Required by the argos module (18 LEDs per half). */
#ifdef RGBLIGHT_LED_COUNT
#    undef RGBLIGHT_LED_COUNT
#endif
#define RGBLIGHT_LED_COUNT 36

/* DPI indicator LED index for bk_pointing_device. */
#ifdef LED_DPI_INDICATOR_INDEX
#    undef LED_DPI_INDICATOR_INDEX
#endif
#define LED_DPI_INDICATOR_INDEX 0

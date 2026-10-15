# TODO — michalvanko charybdis 3x5 keymap

## Fix: num layer breaks after deleting (QUICK_TAP_TERM)

**Want this done: yes, definitely** (just not right now — noted 2025-10-15 while working on the zmk-config cygnus port).

### Symptom

Type numbers (hold `BSP_NUM` thumb) → release and tap the thumb to delete with backspace → press the thumb again to keep typing numbers → digits come out as base-layer characters (e.g. trying to type `888` produces `iii`).

### Root cause

QMK's `QUICK_TAP_TERM` defaults to `TAPPING_TERM` (200 ms): a tap-hold key
re-pressed within that window after a tap is **forced to be another tap**.
`BSP_NUM = LT(LAYER_NUMERAL, KC_BSPC)` re-pressed after a backspace tap
therefore never activates the layer, and the "digit" falls through to the base
layer. QMK also sends interrupting keys immediately on the current layer while
a hold is still pending, which compounds it.

The ZMK keyboards (skeletyl, cygnus) do not have this bug: `quick-tap-ms` is
disabled there and ZMK queues keys pressed during the pending window.

### Fix

Add to `config.h` (this directory):

```c
// Re-pressing a tap-hold key right after tapping it (e.g. holding BSP_NUM
// again right after a backspace tap) must be allowed to trigger the hold,
// otherwise the numeral layer never re-engages while correcting numbers.
#define QUICK_TAP_TERM 0
```

### Verify after flashing

1. Hold right-home thumb → type `789` (numbers appear).
2. Release, tap thumb (backspace deletes).
3. Immediately hold thumb again → type `789` — must be numbers, not `uio`.
4. Double-tapping backspace still deletes twice (tap is unaffected).

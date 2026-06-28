# Tesla CAN × TEIN EDFC5

Tesla Model 3 (pre-2023.09) CAN → TEIN EDFC5 damper control. Board: RejsaCAN-ESP32-S3 v3.4.
Human reference (verbose, JA): `CLAUDE_ja.md`.

## Output
- Reply in Japanese. Terse; fragments over sentences.
- Code comments: Japanese OK.

## Build
- PlatformIO. `pio run -e rejsacan-esp32s3`. Board `esp32-s3-devkitc-1`.
- Arduino core 2.0.17 → LEDC 2.x API (`ledcSetup`/`ledcAttachPin`/`ledcChangeFrequency`).
- `src/` on include path → `#include "shared/..."` works.

## Architecture
- Layers: shared (types/IF) ← gateway (I/O) ← feature (logic). DI via `DIContainer`.
- Feature → gateway only (interface). No feature↔feature deps.
- CAN notify: feature implements `ICanListener`, registers in `setup()` via `addListener(this)`.
- `main.cpp`: DI register + drive `can->begin/update`, `gpio->begin`, `feature->setup/loop`.

## CAN (500kbps, LISTEN_ONLY)
- Speed: ID `0x257`, bits[12:23], ×0.08 −40.0 → kph. Confirmed.
- Accel mode: ID TBD → Phase 1 serial dump. `CAN_MODE_ID`/`parseAccelMode()` = placeholder.

## Pins
- CAN: TX 12 / RX 13 / RS 38.
- LED: blue 10 / yellow 11.
- Relay: EXT1 6 (Sport/hard), EXT2 7 (Chill/soft). Pulse: 15. (6/7/15 provisional)

## Mode → EDFC5 (`AccelMode::applyMode`)
- Chill → EXT2 on, LED blue. Standard → both off, LED blue+yellow. Sport → EXT1 on, LED yellow.

## Excluded (Phase 2)
- `gateway/wifi/`, `features/dashboard/` off via `build_src_filter`. Sources kept.
- `lib_deps` ESP Async WebServer commented (explicit dep = compiled even if unused).

## TODO (after hardware)
- `CAN_MODE_ID` + `parseAccelMode()` bits.
- `PULSES_PER_KM` (4000 placeholder) per EDFC5 manual; relay polarity.
- Confirm provisional pins 6/7/15.

# Tesla CAN × TEIN EDFC5

Tesla Model 3 (pre-2023.09) CAN → TEIN EDFC5 damper control. Board: RejsaCAN-ESP32-S3 v3.4.
Human reference (verbose, JA): `CLAUDE_ja.md`.

## Docs convention
- `CLAUDE.md`: English only, terse — loaded by AI every session (minimize tokens).
- `CLAUDE_ja.md`: Japanese verbose — human reference, NOT loaded automatically.
- Keep both in sync when architecture changes.

## Output
- Reply in Japanese. Terse; fragments over sentences.
- Code comments: Japanese OK.

## Build
- PlatformIO. `pio run -e rejsacan-esp32s3`. Board `esp32-s3-devkitc-1`.
- Arduino core 2.0.17 → LEDC 2.x API (`ledcSetup`/`ledcAttachPin`/`ledcChangeFrequency`).
- `src/` on include path → `#include "shared/..."` works.
- Native unit tests: `pio test -e native`.

## Architecture
- Layers: shared (types/IF) ← gateway (I/O) ← feature (logic). DI via `DIContainer` (Singleton-only).
- Feature → gateway only (interface). No feature↔feature deps.
- CAN notify: feature implements `ICanListener`, registers in `begin()` via `addListener(this)`.
- CAN RX runs in a dedicated FreeRTOS task (core 0). Task started in `CanGateway::begin()`; `loop()` is a no-op.
- Listener callbacks execute on core 0. Keep non-blocking / fast. No GPIO access from core 1 (race condition).
- `main.cpp`: thin bridge — `appInstance()` drives `IApp::setup/loop` via `CompositionRoot`.

## CAN (500kbps, LISTEN_ONLY)
- Speed: ID `0x257`, bits[12:23], ×0.08 −40.0 → kph. Confirmed.
- Accel mode: ID TBD → Phase 1 serial dump. `CAN_MODE_ID`/`parseAccelMode()` = placeholder.

## Pins
- CAN: TX 12 / RX 13 / RS 38.
- LED: blue 10 / yellow 11.
- Relay: EXT1 6 (Sport/hard), EXT2 7 (Chill/soft). Pulse: 15. (6/7/15 provisional)

## Mode → EDFC5 (`AccelMode::applyMode`)
- Chill → EXT2 on, LED blue. Standard → both off, LED blue+yellow. Sport → EXT1 on, LED yellow.

## TODO (after hardware)
- `CAN_MODE_ID` + `parseAccelMode()` bits.
- `PULSES_PER_KM` (4000 placeholder) per EDFC5 manual; relay polarity.
- Confirm provisional pins 6/7/15.

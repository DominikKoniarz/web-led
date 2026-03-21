# AGENTS Guide - ESP Firmware

This file gives firmware-specific context for agents working in `esp/`.

## Firmware Scope

PlatformIO Arduino firmware for ESP32-C6.

Key modules currently:

- `src/main.cpp` - boot and loop orchestration
- `src/wifi_setup.cpp` - Wi-Fi setup/reconnect path
- `src/web_server.cpp` - HTTP API/static serving
- `src/state_service.cpp` - in-memory canonical state service
- `src/state_json_codec.cpp` - typed serializers/deserializers for state JSON

Headers are in `include/`.

## Architecture Intent

Use a central state-first flow:

1. Parse and validate input JSON into typed patch structs.
2. Apply patches to canonical in-memory state.
3. Serialize from typed state for responses/broadcasts.

Avoid direct ad-hoc JSON mutation spread across handlers.

## API Contract Direction

Domain API groups exist under:

- `/api/wifi`
- `/api/led`
- `/api/system`
- `/api/state`
- `/api/contracts`

Keep request and response shape stable unless explicitly changing contract version.

## Change Guidelines

- Prefer keeping state schema compatible across phases.
- Add fields as optional first when possible.
- Keep transport concerns separate from state concerns.
- Do not leak Wi-Fi secrets in logs or API payloads.

## Build and Verification

From repository root:

1. Run `pio run` for compile validation.
2. If API/state shape changed, summarize expected payload updates.

## File Placement Rules

- New firmware logic: `esp/src/`
- Public firmware interfaces: `esp/include/`
- Do not move files across modules unless requested.

## Agents File Discovery Reminder

When working inside firmware, still check parent instructions:

1. Use this `esp/AGENTS.md` as primary for `esp/**`.
2. Also apply repository-level guidance from root `AGENTS.md`.

## Plan: ESP32 Firmware Roadmap for WebLED

Build firmware-only foundations first (state model, Wi-Fi mode manager, WebSocket API, delayed persistence), then layer LED modes and system settings in incremental phases that can be paused and adjusted between milestones. This approach minimizes rework by stabilizing transport and configuration contracts before adding richer effects.

**Steps**

1. Phase 1 - Core State and API Contracts (blocks later phases): Define canonical in-RAM state objects for LED, Wi-Fi, network, and device settings; define JSON message and endpoint contracts for WebSocket and REST domain APIs (/api/wifi, /api/led, /api/system); wire a central state service that all handlers use.
2. Phase 2 - Wi-Fi Mode State Machine and Provisioning (depends on Phase 1): Replace blocking STA connect flow with explicit boot/connect/fallback states; at boot attempt STA reconnect up to 3 times with 15 seconds per attempt, then switch to AP provisioning mode; after the device has previously connected, handle link loss with staged recovery (STA-only retries using 5s -> 10s -> 20s -> 30s max backoff first, then AP+STA rescue mode after 2 minutes disconnected); when STA is stable again for 60 seconds, automatically disable AP rescue mode; expose scan endpoint and connect endpoint for provisioning flow; persist credentials only after successful STA association.
3. Phase 3 - WebSocket Transport and Realtime Sync (parallel with late Phase 2 endpoint polish): This is the phase where WebSocket support is introduced. Implement AsyncWebSocket endpoint, client lifecycle handling, message validation, and state broadcast hooks; ensure LED and settings updates are applied through the central state service and broadcast to all clients while preserving existing HTTP REST endpoints.
4. Phase 4 - LED Engine v1 (depends on Phases 1 and 3): Add LED runtime controller and scheduler tick; implement requested baseline modes (Off, Solid, Rainbow, Breathing) plus extensible mode registry for Chase, Sparkle, Fire, Wave; support enable/disable, brightness, animation speed, and configurable LED count.
5. Phase 5 - Settings, Networking Controls, and Identity (depends on Phases 1 to 3): Add STA static IP (IP/mask/gateway/DNS) configuration path; add device name setting used for AP SSID and mDNS hostname; expose device info and restart endpoint; maintain AP WPA2 security with password webled-setup.
6. Phase 6 - LittleFS Persistence with RAM Cache + Debounce (depends on all prior state fields being finalized): Implement single source-of-truth RAM cache loaded at boot, dirty tracking, and delayed flush to LittleFS (5s debounce); add coalescing so rapid UI updates trigger one write; implement atomic file writes and corruption fallback.
7. Phase 7 - Hardening and Operational Validation (depends on all prior phases): Validate AP fallback reliability, concurrent AP + scan behavior, websocket resilience under reconnect storms, persistence wear behavior, and recovery from malformed payloads.

**Relevant files**

- /Volumes/Programming/Projects/WebLED/esp/src/main.cpp - Replace periodic reconnect-only loop with cooperative update loop for Wi-Fi state machine, websocket maintenance, LED tick, and deferred persistence flush.
- /Volumes/Programming/Projects/WebLED/esp/src/wifi_setup.cpp - Refactor setupWiFi and reconnectWiFi into non-blocking connection manager supporting STA attempts, AP fallback, scan operations, and credential application.
- /Volumes/Programming/Projects/WebLED/esp/include/wifi_setup.h - Expand API surface for mode/state queries and provisioning actions used by REST and WebSocket handlers.
- /Volumes/Programming/Projects/WebLED/esp/src/web_server.cpp - Add domain REST endpoints and WebSocket handler registration; route all state changes through central in-RAM state service.
- /Volumes/Programming/Projects/WebLED/esp/include/web_server.h - Extend declarations for websocket setup/broadcast and richer connected-client reporting.
- /Volumes/Programming/Projects/WebLED/platformio.ini - Add any required libraries for mDNS and LED driver implementation, keeping existing Async server dependency.

**Verification**

1. Boot validation: confirm startup attempts STA connection for 3 attempts x 15s and deterministically enters AP mode when STA fails.
2. Runtime disconnect validation: after a successful STA session, force AP/router outage and verify staged recovery behavior without reboot (STA retries with 5s -> 10s -> 20s -> 30s max backoff, AP+STA rescue starts only after 2 minutes disconnected, AP auto-disables after 60 seconds of stable STA).
3. AP provisioning: in AP mode, scan returns nearby SSIDs and selected SSID credentials trigger immediate connect attempt; credentials persist only after successful connection.
4. Realtime control: WebSocket messages for mode, enabled, brightness, speed, and LED count update runtime state and broadcast synchronized state to all connected clients.
5. Networking settings: STA static IP and subnet settings apply after reconnect; DHCP opt-out and revert path both function.
6. Identity settings: device name updates AP SSID and mDNS hostname after restart/reinit and is discoverable on local network.
7. Persistence: repeated rapid setting changes produce a single delayed LittleFS write per debounce window (5s), with RAM reads serving API responses between writes.
8. Robustness: malformed JSON, unknown commands, and websocket disconnect/reconnect do not crash firmware and produce controlled error responses.
9. System controls: restart endpoint triggers clean reboot and state restore from persisted config.
10. AP provisioning: in AP mode, scan returns nearby SSIDs and selected SSID credentials trigger immediate connect attempt; credentials persist only after successful connection.
11. Realtime control: WebSocket messages for mode, enabled, brightness, speed, and LED count update runtime state and broadcast synchronized state to all connected clients.
12. Networking settings: STA static IP and subnet settings apply after reconnect; DHCP opt-out and revert path both function.
13. Identity settings: device name updates AP SSID and mDNS hostname after restart/reinit and is discoverable on local network.
14. Persistence: repeated rapid setting changes produce a single delayed LittleFS write per debounce window (5s), with RAM reads serving API responses between writes.
15. Robustness: malformed JSON, unknown commands, and websocket disconnect/reconnect do not crash firmware and produce controlled error responses.
16. System controls: restart endpoint triggers clean reboot and state restore from persisted config.

**Decisions**

- Included now: firmware only; no web app changes in this roadmap.
- Included now: LED modes Off, Solid, Rainbow, Breathing as baseline plus extensibility hooks for Chase, Sparkle, Fire, Wave.
- Included now: AP fallback policy based on fixed attempts at boot, with runtime disconnect handled by staged recovery (retry/backoff before AP+STA rescue).
- Included now: AP security WPA2 with password webled.
- Included now: split API domains (/api/wifi, /api/led, /api/system) plus websocket channel.
- Included now: keep HTTP REST endpoints in place even after WebSocket support lands (do not remove them; retain as fallback/control path).
- Included now: static IP support in STA mode only.
- Included now: provisioning flow attempts connect immediately and persists credentials on success.
- Included now: runtime disconnect recovery profile is 2-minute outage threshold, retry backoff 5s -> 10s -> 20s -> 30s max, AP rescue auto-off after 60 seconds stable STA.
- Assumption to confirm during implementation kickoff: reconnecting for 15 seconds means per attempt (3 attempts total).

**Further Considerations**

1. LED backend selection should be finalized before Phase 4 (RMT-based custom driver vs library such as FastLED) because timing/performance differs on ESP32-C6.
2. Consider adding versioned payload schema for WebSocket and REST to keep compatibility while iterating web UI later.
3. Consider credential handling hardening (masking in logs, optional password update endpoint restrictions) before production exposure.

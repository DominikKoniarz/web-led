# AGENTS Guide - WebLED Workspace

This file gives coding agents project-level context and operating rules.

## What This Project Is

WebLED is an ESP32-C6 firmware + web UI project.

- `esp/` contains firmware (PlatformIO, Arduino framework).
- `web/` contains the Vite + React UI.
- Build system uses custom PlatformIO layout paths pointing into `esp/`.

## Agent Context Discovery Rules

Before changing code, always discover agent instruction files in this order:

1. Check for `AGENTS.md` in the current working directory.
2. Check parent directories up to repository root.
3. If entering a major subproject (for example `esp/` or `web/`), look again inside that subproject for a more specific `AGENTS.md`.
4. Apply the nearest file first, then merge with parent guidance.
5. If guidance conflicts, nearest file wins for that subtree.

## Repository Working Norms

- Keep commits scoped and descriptive.
- Do not include unrelated generated files unless user asks.
- Prefer minimal, targeted edits over broad refactors.
- Preserve existing API contracts unless user asks to break them.

## PlatformIO Layout Facts

This repository intentionally uses non-default PlatformIO paths.

- `src_dir = esp/src`
- `include_dir = esp/include`
- `lib_dir = esp/lib`
- `test_dir = esp/test`
- `data_dir = dist/web`

When adding firmware files, place them under `esp/src` and `esp/include`.

## Validation Expectations

For firmware changes:

1. Run `pio run` from repository root.
2. Report success/failure and relevant warnings.

For web changes:

1. Run the applicable workspace command from `web/`.
2. Report success/failure.

## Planning Expectations

For larger work, implement in phases and keep the phase boundary clear in code and commit messages.

Current firmware direction emphasizes:

- central in-RAM state
- explicit API contracts
- incremental transport/runtime features

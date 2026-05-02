# AGENTS Guide - WebLED Workspace

This file gives coding agents project-level context and operating rules.

## What This Project Is

WebLED is an ESP32-C6 firmware + web UI project.

- `esp/` contains firmware (PlatformIO, Arduino framework).
- `web/` contains the Vite + React UI.
- `bruno/` contains Bruno API request collections used to exercise and verify HTTP routes.
- Build system uses custom PlatformIO layout paths pointing into `esp/`.

## API Collection (Bruno)

The `bruno/web-led/` collection should track the firmware HTTP API contract.

- When adding a new HTTP route, add the corresponding Bruno request file(s).
- When updating an existing HTTP route (path, method, request body, or response contract), update the matching Bruno request definitions.
- Keep Bruno examples aligned with current router behavior so manual API verification stays reliable.

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
2. If HTTP router endpoints changed, update the Bruno collection under `bruno/web-led/` to match.
3. If HTTP routes, request bodies, or response schemas changed, update `docs/api/openapi.yaml` to reflect the new contract.
4. After updating OpenAPI spec, run `pnpm run api:docs` to rebuild and verify the documentation generates without errors.
5. Ensure `docs/api/openapi.yaml`, Bruno requests, and firmware implementation stay in sync.
6. Report success/failure and relevant warnings.

For web changes:

1. Run the applicable workspace command from `web/`.
2. Report success/failure.

## API Documentation Maintenance

The HTTP API contract is documented in OpenAPI format at `docs/api/openapi.yaml`.

### When to Update Docs

Update the OpenAPI spec when:

- Adding a new HTTP route to firmware
- Changing request body structure or validation rules
- Changing response schema or status codes
- Renaming fields in request or response payloads
- Adding or removing optional fields

### Update Workflow

1. Make changes to `esp/src/web_server.cpp` (routes) and/or `esp/src/json_codec.cpp` (request/response schemas).
2. Update `docs/api/openapi.yaml` with matching endpoint, request body, and response schema changes.
3. Update Bruno request definitions under `bruno/web-led/` to match new payloads.
4. Build and validate docs: `pnpm run api:docs`
5. Commit all three together: firmware, spec, and Bruno collection.

### Rebuilding Docs

To rebuild the static HTML documentation:

```bash
pnpm run api:docs
```

This will:

- Rebuild `docs/api/index.html` from the OpenAPI spec
- Print the absolute file path and file:// URL for browser access
- Display any validation errors in the spec

## Planning Expectations

For larger work, implement in phases and keep the phase boundary clear in code and commit messages.

Current firmware direction emphasizes:

- central in-RAM state
- explicit API contracts
- incremental transport/runtime features

# WebLED API Docs

The API contract for firmware endpoints is defined in:

- `docs/api/openapi.yaml`

This spec is intended for frontend integration and manual/CI validation.

## Quick Preview

Build docs and print the local absolute path + file URL:

```bash
pnpm run api:docs
```

Build only:

```bash
pnpm run api:docs:build
```

Print path and file URL only:

```bash
pnpm run api:docs:path
```

Direct Redocly command:

```bash
npx redocly build-docs docs/api/openapi.yaml -o docs/api/index.html
```

Then open `docs/api/index.html`.

Alternative local Swagger UI preview:

```bash
npx swagger-ui-watcher docs/api/openapi.yaml --no-open
```

## Update Workflow

1. Change firmware routes or payloads in `esp/src/web_server.cpp` and/or `esp/src/json_codec.cpp`.
2. Update `docs/api/openapi.yaml` in the same change.
3. Update Bruno requests under `bruno/web-led/` when route, body, or response behavior changes.
4. Validate firmware build:

```bash
pio run
```

## Notes

- Validation errors return JSON: `{ "error": "..." }`.
- `POST /api/wifi/connect` returns `204 No Content` on accepted request.
- `GET /api/wifi/scan` is stateful and can return `started`, `running`, or `complete`.

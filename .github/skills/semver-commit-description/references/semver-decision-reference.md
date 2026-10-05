# SemVer Decision Reference

This reference supports classification decisions for commit descriptions.

## MAJOR Signals
- Removed/renamed public API route, RPC, field, or config key
- Changed request/response shape in a non-backward-compatible way
- Behavior change that breaks existing clients by default

## MINOR Signals
- New backward-compatible endpoint/RPC/field
- Optional new capability with old behavior preserved
- New non-breaking configuration option

## PATCH Signals
- Bug fixes preserving external contracts
- Internal refactors, dependency updates, build/test/docs changes
- Performance improvements with no observable contract break

## Tie-break Rule
Choose the highest impact present in the same change set:
- MAJOR over MINOR over PATCH

## Wording Guidance
- Prefer compatibility language: "backward-compatible" or "breaking"
- State affected interface explicitly: route, RPC, schema, CLI, env var, config
- Keep subject imperative and under 72 characters where possible

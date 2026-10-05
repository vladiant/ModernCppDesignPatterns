---
name: semver-commit-description
description: 'Prepare commit descriptions using semantic versioning impact rules. Use when writing commit messages, changelog entries, or release notes that must classify changes as MAJOR, MINOR, or PATCH based on SemVer-style impact.'
argument-hint: 'Describe the change set or point to changed files/diff to classify as major|minor|patch and draft commit description.'
user-invocable: true
disable-model-invocation: false
---

# SemVer Commit Description

Create clear commit descriptions that classify impact using semantic versioning rules from [semver.org](https://semver.org/).

## When to Use
- Preparing a commit message after implementation work
- Summarizing a multi-file change into a single commit description
- Writing release-oriented commit descriptions that state compatibility impact
- Converting technical diffs into MAJOR, MINOR, or PATCH language

## Inputs
- The staged or unstaged diff
- A short summary of user-visible behavior changes
- Any API/schema/config contract changes

## Procedure
1. Build change inventory
- List user-visible behavior changes
- List API contract changes
- List fixes/internal refactors/chore-only edits

2. Classify SemVer impact
- MAJOR: backward-incompatible API or behavior change
- MINOR: backward-compatible new functionality
- PATCH: backward-compatible bug fix or internal non-feature change

3. Apply branching logic for mixed changes
- If any MAJOR condition is present, classify as MAJOR
- Else if any MINOR condition is present, classify as MINOR
- Else classify as PATCH
- If uncertainty remains, produce a primary classification and add a "risk note" in the description

4. Draft commit description
- Subject line format:
  - semver(<major|minor|patch>): <short imperative summary>
- Body format:
  - Why: one or two lines of intent
  - What changed: concise bullets by area
  - Compatibility: explicit statement (breaking or backward-compatible)
  - Validation: tests/build/verification outcome

5. Produce an optional extended release note block
- Impact: MAJOR|MINOR|PATCH
- Affected APIs/components
- Migration steps when breaking

## Decision Checklist
Use this checklist before finalizing classification:
- Does any public API contract change break existing clients?
- Are defaults/behavior changed in a way that can break integrations?
- Are only additive, backward-compatible capabilities introduced?
- Is this strictly a bug fix, refactor, docs, or build/system change?

## Completion Criteria
A result is complete only if all checks pass:
- Exactly one SemVer level is selected (MAJOR/MINOR/PATCH)
- Subject line follows required format
- Compatibility statement is explicit
- Validation status is included
- If MAJOR: migration note is present

## Output Template
Use this template verbatim when requested:

Subject:
semver(<major|minor|patch>): <imperative summary>

Body:
Why:
- <intent>

What changed:
- <change 1>
- <change 2>

Compatibility:
- <breaking|backward-compatible> because <reason>

Validation:
- <tests/build/checks performed>

## Examples
1. Breaking rename of REST route and protobuf field
- semver(major): rename hello endpoint and protobuf response field

2. Add new optional endpoint while preserving existing behavior
- semver(minor): add metrics summary endpoint for operational dashboards

3. Fix timeout handling without API changes
- semver(patch): correct grpc timeout propagation in request pipeline

## References
- [SemVer Decision Reference](./references/semver-decision-reference.md)

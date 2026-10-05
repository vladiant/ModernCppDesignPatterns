---
name: semver-version-publish
description: 'Read the current project version, bump it using SemVer rules derived from commit message titles, update the VERSION file, and publish an annotated git tag. Use when releasing a new version, tagging a release, bumping the package version, or publishing a git tag based on semver.'
argument-hint: 'Optional: force bump level (major|minor|patch), or leave empty to auto-detect from commit messages.'
user-invocable: true
disable-model-invocation: false
---

# SemVer Version Publish

Read the current project version from `VERSION`, determine the correct SemVer bump from commit titles since the last tag, update `VERSION`, and create an annotated git tag.

## When to Use
- Releasing a new version of the project
- Tagging a release commit after changes are merged
- Bumping the package version based on accumulated changes
- Publishing a git tag derived from semver rules
- Auto-detecting whether the release is MAJOR, MINOR, or PATCH

## Inputs
- `VERSION` file at the repo root (plain `MAJOR.MINOR.PATCH` format)
- Commit message titles since the last git tag (or all commits if no tag exists)
- Optional: forced bump level passed as argument (`major`, `minor`, or `patch`)

## Procedure

### Step 1 — Read Current Version
1. Read the `VERSION` file from the repository root.
2. Parse as `MAJOR.MINOR.PATCH` (integers only; no pre-release suffix).
3. Confirm the format is valid before continuing.

### Step 2 — Collect Commits Since Last Tag
1. Run: `git describe --tags --abbrev=0` to find the most recent tag.
   - If the command fails (no tags exist), collect **all** commits: `git log --oneline`.
   - If a tag exists, collect commits since it: `git log <last-tag>..HEAD --oneline`.
2. Extract the subject line of each commit.

### Step 3 — Classify Bump Level

Scan every commit subject using the rules below (highest wins):

| Pattern in subject | Bump |
|--------------------|------|
| `semver(major):` prefix OR `BREAKING CHANGE` anywhere | **MAJOR** |
| `semver(minor):` prefix OR `feat:` / `feat(<scope>):` prefix | **MINOR** |
| `semver(patch):` prefix OR `fix:` / `fix(<scope>):` / `perf:` / `refactor:` / `chore:` / `docs:` / `build:` prefix | **PATCH** |

Priority: MAJOR > MINOR > PATCH.

If a forced bump level was supplied as an argument, skip this step and use the forced level.

If no classifiable commits are found, default to **PATCH** and note this in the output.

### Step 4 — Calculate New Version

Apply the winning bump to the parsed version:

| Bump | Rule |
|------|------|
| MAJOR | Increment MAJOR, reset MINOR=0, PATCH=0 |
| MINOR | Increment MINOR, reset PATCH=0 |
| PATCH | Increment PATCH only |

### Step 5 — Update VERSION File
1. Write the new `MAJOR.MINOR.PATCH` string (followed by a newline) to `VERSION`.
2. Stage the file: `git add VERSION`.

### Step 6 — Create Annotated Git Tag
1. Compose the tag name: `v<MAJOR>.<MINOR>.<PATCH>`.
2. Compose the tag message from the collected commit subjects (one per line), prefixed with the bump classification.
3. Create an annotated tag:
   ```
   git tag -a v<new_version> -m "<tag message>"
   ```
4. Display the tag details: `git show v<new_version> --stat`.

### Step 7 — Confirm Before Pushing
Before running any `git push`, summarize the pending change and ask the user to confirm:
- Current version → new version
- Bump level and reason
- List of commits included
- Tag name and message

Only push after explicit confirmation:
```
git push origin v<new_version>
```

If the user also wants to push the `VERSION` file update as a commit, propose:
```
git commit -m "semver(patch|minor|major): bump version to <new_version>"
git push origin <current-branch>
git push origin v<new_version>
```

## Decision Checklist
Before finalizing, verify:
- [ ] `VERSION` file contains a valid `MAJOR.MINOR.PATCH` string
- [ ] At least one commit exists since the last tag (or since repo init)
- [ ] Bump level is unambiguous, or a reason is stated for the chosen level
- [ ] Tag name matches `v<MAJOR>.<MINOR>.<PATCH>` exactly
- [ ] User has confirmed before any `git push`

## Completion Criteria
A result is complete only when:
- `VERSION` file on disk contains the new version
- Annotated tag exists locally (`git tag -l "v*"` shows it)
- User has been shown a push command and confirmed or declined
- Output includes: old version, new version, bump level, reason, tag name

## Output Template

```
Version bump: <old> → <new>  [<MAJOR|MINOR|PATCH>]

Reason:
- <commit 1 subject>
- <commit 2 subject>
...

Tag created: v<new_version>
Message:
  <bump level>: <summary line>
  ---
  <commit list>

Next step:
  git push origin v<new_version>
```

## Examples

### Auto-detect from commits
Commits since `v0.1.0`:
- `feat: add Prometheus metrics endpoint`
- `fix: correct gRPC timeout handling`

→ Highest bump = MINOR (feat)
→ New version: `0.2.0`
→ Tag: `v0.2.0`

### Forced major bump
Argument: `major`
Current version: `0.2.0`
→ New version: `1.0.0`
→ Tag: `v1.0.0`

### No classifiable commits
Commits: `update README`, `typo fix`
→ Default to PATCH
→ Note: "No semver-prefixed commits found; defaulting to patch bump."

## References
- [semver.org](https://semver.org/) — authoritative versioning rules
- [semver-commit-description skill](../semver-commit-description/SKILL.md) — classifying individual commits
- [Conventional Commits](https://www.conventionalcommits.org/) — `feat:`/`fix:` prefix conventions

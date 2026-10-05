---
name: portfolio-mr-description
description: 'Prepare high-quality pull/merge request descriptions for public portfolio repositories. Use when opening a PR/MR and you need a clear title, value-focused summary, context, grouped change list, verification evidence, and reviewer-friendly checklist.'
argument-hint: 'Provide the change summary/diff context and (optional) linked issue/todo item to draft a complete PR/MR description.'
user-invocable: true
disable-model-invocation: false
---

# Portfolio MR Description

Draft a reviewer-friendly pull/merge request description for portfolio-style repositories where readers may include recruiters, external collaborators, and future maintainers.

## When to Use
- Opening a pull request or merge request for a public/demo/reference project
- Rewriting a weak PR/MR description into a structured, evidence-backed one
- Turning implementation notes into a polished review narrative
- Ensuring a PR/MR history is useful as long-term project documentation

## Inputs
- Branch diff or concise change summary
- Linked issue/task/design note (if available)
- Test and verification results actually executed
- Optional runtime output samples or screenshots

## Procedure

### Step 1 - Build a change inventory
1. Identify the user-visible problem being solved and why it matters for the project.
2. Group changes by concern (feature behavior, build/tooling, docs, tests), not by filename.
3. Extract reviewer-relevant facts:
- API or ABI changes
- New dependencies
- New build targets/CMake options
- Clone-and-run impact (prerequisites, entry point, environment variables)

### Step 2 - Draft a strong title
1. Write one imperative line describing what changed (no trailing period).
2. Keep the title short enough to avoid truncation in lists (target <= 70 chars).
3. Use branching logic:
- If the repository uses Conventional Commits, use `<type>(<scope>): <imperative summary>`.
- Otherwise use plain imperative title text.

### Step 3 - Write summary and motivation/context
1. Summary paragraph (2-4 sentences):
- Explain the problem solved.
- Explain why the change matters to this project.
- Lead with value, not implementation detail.
2. Motivation/context section:
- Link source context (issue, checklist item, or design note).
- Use explicit linkage text like `Closes #<n>` or `Addresses <tracking item>`.
- State known limitations or tradeoffs up front.

### Step 4 - Compose "What changed"
1. Create a concise bullet list grouped by logical area.
2. Include non-obvious reviewer callouts:
- build system changes
- options/flags/default changes
- new external dependencies
- API/ABI compatibility notes
3. For portfolio projects, always mention any change that alters onboarding or run instructions.

### Step 5 - Document verification
1. List exactly what was run (tests, sanitizers, static analysis, manual checks, benchmarks).
2. Use concrete evidence for performance-related claims:
- include before/after numbers
- include environment/conditions when relevant
3. If coverage is incomplete, state what was not tested and why.

### Step 6 - Add artifacts when applicable
Use branching logic:
- If output is visual or user-facing, include before/after screenshots or representative console output.
- If no visible output exists, omit this section.

### Step 7 - Add completion checklist
Use this portfolio-focused checklist unless the repo has an equivalent required format:
- [ ] Builds cleanly with the project's standard build configuration
- [ ] Tests pass locally
- [ ] Documentation/README updated for behavior or usage changes
- [ ] No new warnings introduced by the configured toolchain/checks

## Output Template

Use this template and fill every applicable section:

```markdown
## Title
<imperative title>

## Summary
<2-4 sentence value-first summary>

## Motivation / Context
- <linked issue/checklist/design context>
- <known limitation or tradeoff, if any>

## What Changed
- <grouped change 1>
- <grouped change 2>
- <non-obvious reviewer callout>

## How It Was Tested / Verified
- <unit/integration tests run>
- <sanitizer/static analysis/manual checks>
- <before/after metrics for performance claims, if applicable>
- <explicit test gap, if any>

## Screenshots / Output Samples
<before/after images or console excerpts, if applicable>

## Checklist
- [ ] Builds cleanly with the project's standard build configuration
- [ ] Tests pass locally
- [ ] Documentation / README updated if behavior or usage changed
- [ ] No new warnings introduced by the configured toolchain/checks
```

## Decision Checklist
Before finalizing, verify:
- Title is imperative and concise
- Summary explains both problem and project value
- Context links source task and includes known limitations/tradeoffs
- "What changed" is grouped logically and includes non-obvious impacts
- Verification section lists concrete commands/results (not generic claims)
- Performance claims include before/after data
- Visible-output changes include screenshots/output samples

## Completion Criteria
A PR/MR description is complete only if:
- Required sections are present and non-empty (except screenshots when not applicable)
- Claims are evidence-backed by listed verification
- Reviewer-impacting changes (API, build, dependencies, onboarding) are explicitly called out
- Any incomplete test coverage is disclosed

## Style Guidance
- Prefer short prose plus bullets; optimize for skimming
- Write for a portfolio-public audience (future employers, collaborators, and future maintainers), not only an internal team
- Keep unrelated changes out of one description; focus improves review quality
- If deviating from established repo conventions, state why

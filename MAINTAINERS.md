# Maintainers

## Primary maintainer

- [@MasterRook1e](https://github.com/MasterRook1e)

## Responsibilities

The primary maintainer owns:

- issue triage and reproduction quality
- pull-request review and public API consistency
- numerical and wire-format compatibility decisions
- security and accidental-disclosure reports
- release notes, tags, and compatibility evidence
- preservation of the source-only, game-agnostic repository boundary

## Decision process

Small compatible fixes may be accepted directly after review and CI. A new public API,
wire-format version, numerical model, or dependency should begin with an issue and record
the tradeoffs. Decisions favor the smallest contract that addresses a demonstrated
integration problem.

When reasonable alternatives remain, the maintainer documents the selected option in the
issue or pull request rather than hiding the decision in code.

## Releases

The project follows semantic versioning before 1.0 with explicit alpha status. A release
must not claim Unreal Engine compatibility until `RunUAT BuildPlugin` and relevant
Automation Tests have been recorded for that exact environment.

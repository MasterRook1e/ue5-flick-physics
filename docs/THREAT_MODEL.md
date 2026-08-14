# Threat model and public boundary

This repository is a numerical and engine-integration library. It does not authenticate
users, receive internet traffic by itself, store credentials, or define an authoritative
game protocol. Security work therefore focuses on unsafe input, corrupted transport data,
repository leakage, and misleading compatibility claims.

## Protected assets

- host-process stability
- finite and bounded numerical output
- integrity of compact launch packets against accidental corruption
- separation between reusable public mechanics and product-specific code/content
- accuracy of release and compatibility statements

## Input assumptions

All public numerical inputs may be malformed, non-finite, degenerate, or outside editor
metadata limits. Portable functions reject invalid values or sanitize documented ranges.
Blueprint metadata is treated as editor guidance, not a security boundary.

The six-byte packet may come from an untrusted source. CRC detects accidental corruption
only. Hosts must provide authentication, authorization, rate limiting, replay policy, and
server-authoritative validation when those properties are required.

## Repository leakage controls

Automated validation rejects:

- known product-specific identifiers
- common secret and private-key patterns
- environment files
- generated build directories
- Unreal assets, executables, libraries, archives, and debug symbols
- mismatched public versions
- Unreal wrappers that stop delegating to the portable source of truth

These checks reduce risk but do not replace human review. Contributors must not submit
private history, proprietary assets, credentials, or third-party code without compatible
rights.

## Out of scope

- cryptographic protocol design
- cheat prevention
- authoritative multiplayer simulation
- sandboxing hostile C++ callers
- protection against a compromised host process
- ownership or licensing review of a consuming project

## Reporting

Potential vulnerabilities or accidental private-content exposure should follow
[SECURITY.md](../SECURITY.md). Reports should use minimal synthetic reproductions and must
not attach credentials or private source.

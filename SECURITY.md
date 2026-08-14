# Security policy

Flick Physics does not handle accounts, credentials, payments, or network connections by
itself. Security-relevant concerns include malformed numerical input, corrupted command
packets, unsafe integration guidance, and accidental publication of private material.

## Supported versions

Security fixes are applied to the latest source on `main` while the project is pre-1.0.
Version-specific support begins after the first engine-verified release.

## Reporting

For an issue that can be disclosed safely, open a minimal GitHub issue without private
source, credentials, or exploit secrets. For accidental private-content exposure or a
vulnerability whose publication would create immediate risk, use GitHub's private
security-reporting path when available.

Include:

- affected commit or version
- minimal synthetic reproduction
- expected and actual behavior
- impact and preconditions
- suggested mitigation when known

The maintainer will acknowledge, triage, and communicate a remediation plan as repository
availability permits. No fixed response-time SLA is promised.

See `docs/THREAT_MODEL.md` for scope and explicit non-goals.

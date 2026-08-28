# Roadmap

The roadmap prioritizes verified usefulness over feature count. New numerical contracts
must remain generic and must have a concrete integration use case.

## 0.5: impact evidence and attribution

- [x] Relative, normal, and tangential impact metrics
- [x] Reduced-mass momentum and normal-energy evidence
- [x] Weighted dead zones and bounded response curves
- [x] Root, immediate-source, carrier, action, generation, and depth provenance
- [x] Bounded provenance transfer and canonical unordered pair keys
- [x] Team-independent directional source selection with ambiguity rejection
- [x] Arbitrary-plane projection for planar interactions
- [x] Focused regressions and 10,000 fixed-seed impact-pair properties
- [x] Deterministic source release with checksum and build provenance

## 0.4: lifecycle and telemetry

- [x] Deterministic launched/moving/settling/terminal state machine
- [x] Separate start and settle thresholds
- [x] Minimum active duration and stable-window completion
- [x] Optional timeout and completion reason
- [x] Distance, displacement, peak-speed, elapsed-time, and sample telemetry
- [x] Portable regression and property tests
- [x] Unreal and Blueprint wrappers
- [x] Post-physics lifecycle component with opt-in body mutation

## 0.3: portable and reproducible core

- [x] Header-only C++17 numerical source of truth
- [x] Four launch response curves and direction snapping
- [x] Ray/plane projection
- [x] Damped trajectory and inverse target solver
- [x] Fixed-width command packet with CRC
- [x] Stable motion-state contract
- [x] Property tests, sanitizers, CodeQL, CMake install, and consumer smoke test
- [x] Unreal and Blueprint wrappers for the portable contracts

## Release readiness

- [x] Publish a portable source release with deterministic archive, checksum, and provenance
- [ ] Run `BuildPlugin` against the declared UE 5.8 environment
- [ ] Execute the `FlickPhysics` Automation Test prefix
- [ ] Record the first verified compatibility row
- [ ] Publish an original minimal integration fixture and short visual demonstration
- [ ] Create the first engine-verified release package

## Adoption work

- [ ] Add a concise migration guide from direct impulse code
- [ ] Collect real external integration feedback through issues
- [ ] Add reflected impact wrappers only after public engine-build evidence exists
- [ ] Add only compatibility fixes or generic features requested by real consumers
- [ ] Document measured benchmark methodology without universal performance claims

## Explicitly out of scope

- a complete game framework
- characters, factions, combat, progression, or content
- custom networking authentication or authoritative simulation
- proprietary assets or private product logic

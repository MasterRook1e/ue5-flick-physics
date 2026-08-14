# Roadmap

The roadmap prioritizes verified usefulness over feature count. New numerical contracts
must remain generic and must have a concrete integration use case.

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

- [x] Provide a repository-safe one-command Windows verification harness
- [ ] Run `BuildPlugin` against the declared UE 5.8 environment
- [ ] Execute the `FlickPhysics` Automation Test prefix
- [ ] Record the first verified compatibility row
- [ ] Publish an original minimal integration fixture and short visual demonstration
- [ ] Create the first engine-verified release package

## Adoption work

- [ ] Add a concise migration guide from direct impulse code
- [ ] Collect real external integration feedback through issues
- [ ] Add only compatibility fixes or generic features requested by real consumers
- [ ] Document measured benchmark methodology without universal performance claims

## Explicitly out of scope

- a complete game framework
- characters, factions, combat, progression, or content
- custom networking authentication or authoritative simulation
- proprietary assets or private product logic

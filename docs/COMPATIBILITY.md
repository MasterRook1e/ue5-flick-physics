# Compatibility evidence

Compatibility is recorded only after a reproducible command has actually run. A source
file that looks compatible is not counted as build evidence.

## Portable C++17 core

The GitHub Actions matrix compiles and tests the portable core on:

| Runner | Toolchain supplied by runner | Validation |
|---|---|---|
| Ubuntu latest | default GNU/Clang environment | strict warnings, tests, ASan, UBSan, install, consumer build |
| Windows latest | MSVC environment | strict warnings, tests, install, consumer build |
| macOS latest | AppleClang environment | strict warnings, tests, install, consumer build |

Exact runner images and compiler versions are visible in each workflow log. They are not
hard-coded here because hosted runner images change over time.

## Unreal Engine plugin

Run the public verification harness on a Windows machine with the target engine:

```powershell
scripts\verify_unreal_windows.ps1 -EngineRoot "D:\UE_5.8"
```

The harness packages the plugin, creates a neutral temporary host, executes the
`FlickPhysics` test prefix, and writes a summary outside the repository. See
[UNREAL_VERIFICATION.md](UNREAL_VERIFICATION.md).

| Engine | Platform | BuildPlugin | Automation Tests | Status |
|---|---|---:|---:|---|
| UE 5.8.x | Windows | pending | pending | not yet verified publicly |

A row becomes verified only when the repository records:

1. exact engine version or changelist
2. platform and toolchain
3. `RunUAT BuildPlugin` command and successful result
4. `FlickPhysics` Automation Test command and successful result
5. commit SHA tested

Until then, the plugin remains alpha and the README does not claim verified engine support.

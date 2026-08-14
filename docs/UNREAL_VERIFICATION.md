# Unreal Engine verification harness

The Windows harness closes the gap between portable C++ validation and a real Unreal
Engine plugin build. It uses only this public repository and a generated neutral host; it
does not require or inspect a private game project.

## What it verifies

1. discovers a usable Unreal Engine installation and records `Build.version`
2. optionally runs the repository public-boundary validator
3. runs `RunUAT BuildPlugin` for the requested target platforms
4. copies the packaged plugin into a temporary content-only host project
5. launches `UnrealEditor-Cmd.exe` with `Automation RunTest FlickPhysics`
6. requires a successful process exit and completed `FlickPhysics.*` test records
7. requires JSON/HTML Automation Report output
8. writes machine-readable and review-friendly verification summaries

Epic documents `Automation RunTest`, `-ReportExportPath`, and the log/result formats used
by this harness. A test process exit code of zero and successful completed-test records are
both required; source test files alone are not treated as executed evidence.

## Run it

From a repository clone on Windows:

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File scripts/verify_unreal_windows.ps1 `
  -EngineRoot "D:\UE_5.8"
```

Or from Command Prompt:

```bat
scripts\verify_unreal_windows.cmd -EngineRoot "D:\UE_5.8"
```

The harness also checks common Epic Launcher locations, launcher manifests, custom-build
registry entries, `UE_ENGINE_ROOT`, and `UE_5_8_ROOT`. Pass `-EngineRoot` when several
installations exist.

By default, the expected engine minor is `5.8`. To exercise another installed engine for
investigation without claiming it as the release target:

```powershell
scripts\verify_unreal_windows.ps1 `
  -EngineRoot "D:\UE_5.7" `
  -ExpectedEngineMinor "5.8" `
  -AllowOtherEngineVersion
```

## Output

The default output is a timestamped directory under `%TEMP%`, never inside the repository:

```text
FlickPhysicsVerification-YYYYMMDD-HHMMSS/
├── PackagedPlugin/
├── HostProject/
├── AutomationReport/
├── BuildPlugin.log
├── Automation.log
├── Automation.stdout.log
├── verification-summary.json
└── verification-summary.md
```

`verification-summary.md` intentionally omits absolute engine and workspace paths and is
the safest artifact to quote in an issue or compatibility update. Review full native logs
before publishing them because tool output can contain local filesystem paths.

## Recording compatibility

After a successful run:

1. preserve `verification-summary.json`, the report, and logs outside the repository
2. update `docs/COMPATIBILITY.md` with engine version, changelist, platform, tested commit,
   BuildPlugin result, and Automation Test count
3. link the public issue or CI artifact containing reviewed evidence
4. only then describe that engine/platform pair as verified

Do not publish a private project, private source, credentials, or proprietary assets as
compatibility evidence.

## Troubleshooting

- **Engine not found:** pass `-EngineRoot` or set `UE_ENGINE_ROOT`.
- **BuildPlugin fails:** review `BuildPlugin.log`; verify Visual Studio components and the
  Windows SDK required by the installed engine.
- **No tests discovered:** confirm the packaged plugin was copied into the generated host
  and that the build includes editor automation tests.
- **Automation exits non-zero:** review `Automation.log` and the exported report; the
  harness does not reinterpret a failing engine exit as success.
- **Output rejected:** choose a directory outside the repository to avoid accidental
  commits of binaries, reports, and local paths.

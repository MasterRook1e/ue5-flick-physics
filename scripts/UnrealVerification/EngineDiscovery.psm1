Set-StrictMode -Version Latest

function Resolve-FullPath {
    param([Parameter(Mandatory = $true)][string]$Path)
    return [System.IO.Path]::GetFullPath($Path)
}

function Test-IsPathInside {
    param(
        [Parameter(Mandatory = $true)][string]$Candidate,
        [Parameter(Mandatory = $true)][string]$Parent
    )

    $candidateFull = (Resolve-FullPath $Candidate).TrimEnd([char[]]@('\', '/'))
    $parentFull = (Resolve-FullPath $Parent).TrimEnd([char[]]@('\', '/'))
    return $candidateFull.Equals($parentFull, [System.StringComparison]::OrdinalIgnoreCase) -or
        $candidateFull.StartsWith(
            $parentFull + [System.IO.Path]::DirectorySeparatorChar,
            [System.StringComparison]::OrdinalIgnoreCase)
}

function Add-EngineCandidate {
    param(
        [System.Collections.Generic.List[string]]$Candidates,
        [string]$Candidate
    )

    if ([string]::IsNullOrWhiteSpace($Candidate)) {
        return
    }
    try {
        $full = Resolve-FullPath $Candidate
    }
    catch {
        return
    }
    if (-not $Candidates.Contains($full)) {
        [void]$Candidates.Add($full)
    }
}

function Get-EngineCandidates {
    param([string]$ExplicitRoot)

    $candidates = [System.Collections.Generic.List[string]]::new()
    Add-EngineCandidate $candidates $ExplicitRoot
    Add-EngineCandidate $candidates $env:UE_ENGINE_ROOT
    Add-EngineCandidate $candidates $env:UE_5_8_ROOT

    foreach ($path in @(
        "D:\UE_5.8",
        "D:\UE_5.8.1",
        "C:\Program Files\Epic Games\UE_5.8",
        "C:\Program Files\Epic Games\UE_5.8EA",
        "E:\UE_5.8",
        "E:\UE_5.8.1"
    )) {
        Add-EngineCandidate $candidates $path
    }

    $manifestRoot = Join-Path $env:ProgramData "Epic\EpicGamesLauncher\Data\Manifests"
    if (Test-Path -LiteralPath $manifestRoot) {
        foreach ($manifest in Get-ChildItem -LiteralPath $manifestRoot -Filter "*.item" -File -ErrorAction SilentlyContinue) {
            try {
                $item = Get-Content -LiteralPath $manifest.FullName -Raw | ConvertFrom-Json
                $display = [string]$item.DisplayName
                $appName = [string]$item.AppName
                if ($display -match "Unreal Engine" -or $appName -match "^UE_") {
                    Add-EngineCandidate $candidates ([string]$item.InstallLocation)
                }
            }
            catch {
                Write-Verbose "Ignoring unreadable Epic manifest: $($manifest.FullName)"
            }
        }
    }

    foreach ($registryPath in @(
        "HKCU:\Software\Epic Games\Unreal Engine\Builds",
        "HKLM:\Software\EpicGames\Unreal Engine"
    )) {
        if (-not (Test-Path -LiteralPath $registryPath)) {
            continue
        }
        try {
            $properties = Get-ItemProperty -LiteralPath $registryPath
            foreach ($property in $properties.PSObject.Properties) {
                if ($property.Name -notmatch "^PS" -and $property.Value -is [string]) {
                    Add-EngineCandidate $candidates ([string]$property.Value)
                }
            }
        }
        catch {
            Write-Verbose "Ignoring unreadable registry path: $registryPath"
        }
    }

    return $candidates
}

function Get-EngineInstallation {
    param(
        [string]$ExplicitRoot,
        [string]$ExpectedMinor,
        [switch]$AllowOther
    )

    $valid = @()
    foreach ($candidate in Get-EngineCandidates $ExplicitRoot) {
        $runUat = Join-Path $candidate "Engine\Build\BatchFiles\RunUAT.bat"
        $editorCmd = Join-Path $candidate "Engine\Binaries\Win64\UnrealEditor-Cmd.exe"
        $buildVersionPath = Join-Path $candidate "Engine\Build\Build.version"
        if (-not (Test-Path -LiteralPath $runUat) -or
            -not (Test-Path -LiteralPath $editorCmd) -or
            -not (Test-Path -LiteralPath $buildVersionPath)) {
            continue
        }

        try {
            $buildVersion = Get-Content -LiteralPath $buildVersionPath -Raw | ConvertFrom-Json
            $minor = "$($buildVersion.MajorVersion).$($buildVersion.MinorVersion)"
            $valid += [pscustomobject]@{
                Root = $candidate
                RunUat = $runUat
                EditorCmd = $editorCmd
                BuildVersion = $buildVersion
                MinorVersion = $minor
                FullVersion = "$minor.$($buildVersion.PatchVersion)"
            }
        }
        catch {
            Write-Warning "Ignoring engine with unreadable Build.version: $candidate"
        }
    }

    $matching = @($valid | Where-Object { $_.MinorVersion -eq $ExpectedMinor })
    if ($matching.Count -gt 0) {
        return $matching[0]
    }
    if ($AllowOther -and $valid.Count -gt 0) {
        Write-Warning "Expected UE $ExpectedMinor, using UE $($valid[0].FullVersion) because -AllowOtherEngineVersion was supplied."
        return $valid[0]
    }

    $searched = (Get-EngineCandidates $ExplicitRoot) -join "`n  - "
    throw "Unable to locate a usable Unreal Engine $ExpectedMinor installation. Searched:`n  - $searched`nPass -EngineRoot explicitly or set UE_ENGINE_ROOT."
}

function Invoke-RepositoryValidation {
    param([string]$RepositoryRoot)

    $validator = Join-Path $RepositoryRoot "scripts\validate_repository.py"
    if (-not (Test-Path -LiteralPath $validator)) {
        throw "Repository validator was not found: $validator"
    }

    foreach ($candidate in @(
        [pscustomobject]@{ Command = "python"; Arguments = @() },
        [pscustomobject]@{ Command = "py"; Arguments = @("-3") }
    )) {
        $runtime = Get-Command $candidate.Command -ErrorAction SilentlyContinue
        if ($null -eq $runtime) {
            continue
        }
        $runtimeArguments = @($candidate.Arguments) + @($validator)
        & $runtime.Source @runtimeArguments | ForEach-Object { Write-Host $_ }
        $exitCode = $LASTEXITCODE
        if ($exitCode -ne 0) {
            throw "Repository validation failed."
        }
        return ($candidate.Command + $(if ($candidate.Arguments.Count) { " -3" } else { "" }))
    }

    Write-Warning "Python was not found; repository validation was skipped. GitHub CI still performs this check."
    return "not available"
}

function Get-GitCommit {
    param([string]$RepositoryRoot)

    $git = Get-Command git -ErrorAction SilentlyContinue
    if ($null -eq $git) {
        return "unknown"
    }
    try {
        $commit = (& $git.Source -C $RepositoryRoot rev-parse HEAD 2>$null).Trim()
        if ($LASTEXITCODE -eq 0 -and $commit -match "^[0-9a-f]{40}$") {
            return $commit
        }
    }
    catch {
        return "unknown"
    }
    return "unknown"
}

Export-ModuleMember -Function Resolve-FullPath, Test-IsPathInside, Get-EngineInstallation, Invoke-RepositoryValidation, Get-GitCommit

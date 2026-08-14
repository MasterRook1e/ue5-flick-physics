Set-StrictMode -Version Latest

function Invoke-NativeLogged {
    param(
        [Parameter(Mandatory = $true)][string]$Executable,
        [Parameter(Mandatory = $true)][string[]]$Arguments,
        [Parameter(Mandatory = $true)][string]$LogPath
    )

    New-Item -ItemType Directory -Path (Split-Path -Parent $LogPath) -Force | Out-Null
    Write-Host "$Executable $($Arguments -join ' ')"
    & $Executable @Arguments 2>&1 |
        Tee-Object -FilePath $LogPath |
        ForEach-Object { Write-Host $_ }
    $exitCode = [int]$LASTEXITCODE
    return $exitCode
}

function New-VerificationHost {
    param(
        [string]$HostRoot,
        [string]$PackagedPluginRoot
    )

    New-Item -ItemType Directory -Path $HostRoot -Force | Out-Null
    $pluginDestination = Join-Path $HostRoot "Plugins\FlickPhysics"
    New-Item -ItemType Directory -Path $pluginDestination -Force | Out-Null
    Copy-Item -Path (Join-Path $PackagedPluginRoot "*") -Destination $pluginDestination -Recurse -Force

    $projectPath = Join-Path $HostRoot "FlickPhysicsVerification.uproject"
    $projectJson = [ordered]@{
        FileVersion = 3
        Category = "Verification"
        Description = "Temporary neutral host used to verify the public Flick Physics plugin."
        Plugins = @([ordered]@{ Name = "FlickPhysics"; Enabled = $true })
    } | ConvertTo-Json -Depth 8
    [System.IO.File]::WriteAllText(
        $projectPath,
        $projectJson,
        [System.Text.UTF8Encoding]::new($false))
    return $projectPath
}

function Get-AutomationEvidence {
    param([string]$LogPath)

    if (-not (Test-Path -LiteralPath $LogPath)) {
        throw "Automation log was not created: $LogPath"
    }

    $log = Get-Content -LiteralPath $LogPath -Raw
    $options = [System.Text.RegularExpressions.RegexOptions]::IgnoreCase -bor
        [System.Text.RegularExpressions.RegexOptions]::Multiline
    $pattern = [regex]::new(
        "Test Completed\. Result=\{(?<Result>[^}]+)\}.*Path=\{(?<Path>FlickPhysics[^}]*)\}",
        $options)
    $matches = $pattern.Matches($log)
    if ($matches.Count -eq 0) {
        throw "No completed FlickPhysics Automation Tests were found in the log."
    }

    $failed = @($matches | Where-Object {
        $_.Groups["Result"].Value -notmatch "^(Success|Passed)$"
    })
    if ($failed.Count -gt 0 -or $log -match "(?i)Automation Test Failed") {
        $failedNames = @($failed | ForEach-Object { $_.Groups["Path"].Value }) -join ", "
        throw "One or more FlickPhysics Automation Tests failed: $failedNames"
    }

    return [pscustomobject]@{
        Completed = $matches.Count
        Failed = 0
        Paths = @($matches | ForEach-Object { $_.Groups["Path"].Value })
    }
}

function Write-VerificationSummary {
    param(
        [string]$DestinationRoot,
        [object]$Engine,
        [string]$PluginVersion,
        [string]$Commit,
        [string]$ValidationCommand,
        [int]$BuildExitCode,
        [int]$AutomationExitCode,
        [object]$Evidence,
        [string]$TargetPlatform
    )

    $summary = [ordered]@{
        SchemaVersion = 1
        VerifiedAtUtc = [DateTime]::UtcNow.ToString("o")
        RepositoryCommit = $Commit
        PluginVersion = $PluginVersion
        Engine = [ordered]@{
            Version = $Engine.FullVersion
            Changelist = [int64]$Engine.BuildVersion.Changelist
            CompatibleChangelist = [int64]$Engine.BuildVersion.CompatibleChangelist
            BranchName = [string]$Engine.BuildVersion.BranchName
        }
        Platform = [ordered]@{
            OperatingSystem = [System.Environment]::OSVersion.VersionString
            Architecture = [System.Runtime.InteropServices.RuntimeInformation]::OSArchitecture.ToString()
            TargetPlatforms = $TargetPlatform
            PowerShell = $PSVersionTable.PSVersion.ToString()
        }
        RepositoryValidation = $ValidationCommand
        BuildPlugin = [ordered]@{
            Succeeded = $BuildExitCode -eq 0
            ExitCode = $BuildExitCode
            Log = "BuildPlugin.log"
            PackageDirectory = "PackagedPlugin"
        }
        Automation = [ordered]@{
            Succeeded = $AutomationExitCode -eq 0 -and $Evidence.Failed -eq 0
            ExitCode = $AutomationExitCode
            CompletedTests = $Evidence.Completed
            FailedTests = $Evidence.Failed
            TestPaths = $Evidence.Paths
            Log = "Automation.log"
            ReportDirectory = "AutomationReport"
        }
    }

    $jsonPath = Join-Path $DestinationRoot "verification-summary.json"
    $summary | ConvertTo-Json -Depth 12 | Set-Content -LiteralPath $jsonPath -Encoding UTF8

    $markdownPath = Join-Path $DestinationRoot "verification-summary.md"
    $shortCommit = if ($Commit.Length -ge 12) { $Commit.Substring(0, 12) } else { $Commit }
    @"
# Flick Physics Unreal verification

- Verified at (UTC): $($summary.VerifiedAtUtc)
- Repository commit: ``$shortCommit``
- Plugin version: ``$PluginVersion``
- Unreal Engine: ``$($Engine.FullVersion)`` (CL $($Engine.BuildVersion.Changelist))
- Target platform: ``$TargetPlatform``
- BuildPlugin: **passed**
- Automation Tests: **passed** ($($Evidence.Completed) completed, 0 failed)

The full logs and JSON/HTML test report remain in the verification output directory. Review
logs before publishing them because native tool output can contain local filesystem paths.
"@ | Set-Content -LiteralPath $markdownPath -Encoding UTF8

    return [pscustomobject]@{ Json = $jsonPath; Markdown = $markdownPath }
}

Export-ModuleMember -Function Invoke-NativeLogged, New-VerificationHost, Get-AutomationEvidence, Write-VerificationSummary

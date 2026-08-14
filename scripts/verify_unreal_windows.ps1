#requires -Version 5.1

[CmdletBinding()]
param(
    [string]$EngineRoot = "",
    [string]$OutputRoot = "",
    [string]$ExpectedEngineMinor = "5.8",
    [string]$TargetPlatforms = "Win64",
    [switch]$AllowOtherEngineVersion,
    [switch]$SkipRepositoryValidation
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest
Import-Module (Join-Path $PSScriptRoot "UnrealVerification\EngineDiscovery.psm1") -Force
Import-Module (Join-Path $PSScriptRoot "UnrealVerification\Execution.psm1") -Force

function Write-Step {
    param([string]$Message)
    Write-Host "`n==> $Message" -ForegroundColor Cyan
}

$repositoryRoot = Resolve-FullPath (Join-Path $PSScriptRoot "..")
$pluginDescriptor = Join-Path $repositoryRoot "FlickPhysics.uplugin"
if (-not (Test-Path -LiteralPath $pluginDescriptor)) {
    throw "FlickPhysics.uplugin was not found at the repository root."
}

if ([string]::IsNullOrWhiteSpace($OutputRoot)) {
    $OutputRoot = Join-Path $env:TEMP ("FlickPhysicsVerification-" + (Get-Date -Format "yyyyMMdd-HHmmss"))
}
$outputFull = Resolve-FullPath $OutputRoot
if (Test-IsPathInside $outputFull $repositoryRoot) {
    throw "Verification output must be outside the repository: $outputFull"
}
New-Item -ItemType Directory -Path $outputFull -Force | Out-Null

Write-Step "Discover Unreal Engine"
$engine = Get-EngineInstallation $EngineRoot $ExpectedEngineMinor -AllowOther:$AllowOtherEngineVersion
Write-Host "Using Unreal Engine $($engine.FullVersion) (CL $($engine.BuildVersion.Changelist))"
Write-Host "Engine root: $($engine.Root)"

$pluginMetadata = Get-Content -LiteralPath $pluginDescriptor -Raw | ConvertFrom-Json
$pluginVersion = [string]$pluginMetadata.VersionName
$commit = Get-GitCommit $repositoryRoot
$validationCommand = "skipped by option"
if (-not $SkipRepositoryValidation) {
    Write-Step "Validate public repository boundary"
    $validationCommand = Invoke-RepositoryValidation $repositoryRoot
}

$packageDirectory = Join-Path $outputFull "PackagedPlugin"
$buildLog = Join-Path $outputFull "BuildPlugin.log"
if (Test-Path -LiteralPath $packageDirectory) {
    Remove-Item -LiteralPath $packageDirectory -Recurse -Force
}

Write-Step "Package plugin with Unreal AutomationTool"
$buildArguments = @(
    "BuildPlugin",
    "-Plugin=$pluginDescriptor",
    "-Package=$packageDirectory",
    "-TargetPlatforms=$TargetPlatforms"
)
$buildExitCode = Invoke-NativeLogged $engine.RunUat $buildArguments $buildLog
if ($buildExitCode -ne 0) {
    throw "BuildPlugin failed with exit code $buildExitCode. See $buildLog"
}

$packagedDescriptors = @(Get-ChildItem -LiteralPath $packageDirectory -Filter "FlickPhysics.uplugin" -File -Recurse)
if ($packagedDescriptors.Count -ne 1) {
    throw "Expected exactly one packaged FlickPhysics.uplugin, found $($packagedDescriptors.Count)."
}
$packagedPluginRoot = Split-Path -Parent $packagedDescriptors[0].FullName

Write-Step "Create neutral temporary host project"
$hostRoot = Join-Path $outputFull "HostProject"
if (Test-Path -LiteralPath $hostRoot) {
    Remove-Item -LiteralPath $hostRoot -Recurse -Force
}
$hostProject = New-VerificationHost $hostRoot $packagedPluginRoot

Write-Step "Run FlickPhysics Automation Tests"
$automationReport = Join-Path $outputFull "AutomationReport"
$automationLog = Join-Path $outputFull "Automation.log"
$automationStdoutLog = Join-Path $outputFull "Automation.stdout.log"
New-Item -ItemType Directory -Path $automationReport -Force | Out-Null
$automationArguments = @(
    $hostProject,
    "-unattended",
    "-nop4",
    "-nullrhi",
    "-nosplash",
    "-nosound",
    "-stdout",
    "-FullStdOutLogOutput",
    "-NoPause",
    "-ExecCmds=Automation RunTest FlickPhysics;Quit",
    "-TestExit=Automation Test Queue Empty",
    "-ReportExportPath=$automationReport",
    "-abslog=$automationLog"
)
$automationExitCode = Invoke-NativeLogged $engine.EditorCmd $automationArguments $automationStdoutLog
if ($automationExitCode -ne 0) {
    throw "Automation Tests exited with code $automationExitCode. See $automationLog"
}

$evidence = Get-AutomationEvidence $automationLog
if (@(Get-ChildItem -LiteralPath $automationReport -File -Recurse -ErrorAction SilentlyContinue).Count -eq 0) {
    throw "Automation Tests produced no JSON/HTML report files in $automationReport"
}

Write-Step "Write verification evidence"
$summaryArguments = @{
    DestinationRoot = $outputFull
    Engine = $engine
    PluginVersion = $pluginVersion
    Commit = $commit
    ValidationCommand = $validationCommand
    BuildExitCode = $buildExitCode
    AutomationExitCode = $automationExitCode
    Evidence = $evidence
    TargetPlatform = $TargetPlatforms
}
$summaryPaths = Write-VerificationSummary @summaryArguments

Write-Host "`nFlick Physics Unreal verification passed." -ForegroundColor Green
Write-Host "Summary: $($summaryPaths.Markdown)"
Write-Host "Machine-readable summary: $($summaryPaths.Json)"
Write-Host "Output directory: $outputFull"

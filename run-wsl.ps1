param(
    [string]$Distro = "Ubuntu-22.04",
    [ValidateSet("Debug", "Release", "RelWithDebInfo", "MinSizeRel")]
    [string]$BuildType = "Debug",
    [switch]$RunTests,
    [switch]$NoRun,
    [switch]$SkipPackageInstall
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Test-IsAdministrator {
    $identity = [Security.Principal.WindowsIdentity]::GetCurrent()
    $principal = [Security.Principal.WindowsPrincipal]::new($identity)
    return $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)
}

function Start-WslFeatureInstall {
$command = @"
dism.exe /online /enable-feature /featurename:Microsoft-Windows-Subsystem-Linux /all /norestart
dism.exe /online /enable-feature /featurename:VirtualMachinePlatform /all /norestart
if (Get-Command wsl.exe -ErrorAction SilentlyContinue) {
    wsl.exe --set-default-version 2
    wsl.exe --install -d $Distro
}
else {
    Write-Host 'wsl.exe is not available yet. Reboot Windows, then rerun .\run-wsl.ps1.'
}
Write-Host ''
Write-Host 'If Windows asks for reboot, reboot and run .\run-wsl.ps1 again.'
"@

    if (Test-IsAdministrator) {
        powershell.exe -NoProfile -ExecutionPolicy Bypass -Command $command
    }
    else {
        Write-Host "WSL is not installed. Starting elevated PowerShell to enable WSL and install $Distro..."
        Start-Process powershell.exe -Verb RunAs -ArgumentList @(
            "-NoExit",
            "-NoProfile",
            "-ExecutionPolicy", "Bypass",
            "-Command", $command
        )
    }

    Write-Host "After WSL installation completes, open the distro once to create a Linux user, then rerun this script."
    exit 0
}

function Convert-ToWslPath {
    param([Parameter(Mandatory = $true)][string]$WindowsPath)

    $fullPath = (Resolve-Path -LiteralPath $WindowsPath).Path
    if ($fullPath -notmatch "^[A-Za-z]:\\") {
        throw "Cannot convert path to WSL format: $fullPath"
    }

    $drive = $fullPath.Substring(0, 1).ToLowerInvariant()
    $rest = $fullPath.Substring(2).Replace("\", "/")
    return "/mnt/$drive$rest"
}

function Invoke-Wsl {
    param([Parameter(Mandatory = $true)][string]$Command)

    $scriptText = "set -e`n$Command"
    $encoded = [Convert]::ToBase64String([Text.Encoding]::UTF8.GetBytes($scriptText))
    $launcher = "echo $encoded | base64 -d | bash"
    $arguments = @("-d", $script:Distro, "--cd", $script:WslProjectRoot, "--", "bash", "-lc", $launcher)
    & wsl.exe @arguments
    if ($LASTEXITCODE -ne 0) {
        throw "WSL command failed with exit code $LASTEXITCODE"
    }
}

function Invoke-WslProgram {
    param([Parameter(Mandatory = $true)][string[]]$CommandArgs)

    $arguments = @("-d", $script:Distro, "--cd", $script:WslProjectRoot, "--") + $CommandArgs
    & wsl.exe @arguments
    if ($LASTEXITCODE -ne 0) {
        throw "WSL program failed with exit code $LASTEXITCODE"
    }
}

function Get-WslDistros {
    $raw = & wsl.exe -l -q 2>$null
    if ($LASTEXITCODE -ne 0) {
        return @()
    }

    return @(
        $raw |
            ForEach-Object { ($_ -replace [char]0, "").Trim() } |
            Where-Object { $_.Length -gt 0 }
    )
}

$wslCommand = Get-Command wsl.exe -ErrorAction SilentlyContinue
if ($null -eq $wslCommand) {
    Start-WslFeatureInstall
}

$distros = Get-WslDistros
if ($distros -notcontains $Distro) {
    Write-Host "WSL distro '$Distro' was not found."
    Write-Host "Available distros: $($distros -join ', ')"
    Write-Host "Installing $Distro..."
    & wsl.exe --install -d $Distro
    if ($LASTEXITCODE -ne 0) {
        throw "Failed to install WSL distro '$Distro'. Try running PowerShell as Administrator and rerun this script."
    }

    Write-Host "Open $Distro once to finish Linux user setup, then rerun this script."
    exit 0
}

$script:WslProjectRoot = Convert-ToWslPath $PSScriptRoot

Write-Host "Project: $PSScriptRoot"
Write-Host "WSL path: $script:WslProjectRoot"
Write-Host "Distro: $Distro"

if (-not $SkipPackageInstall) {
    Invoke-Wsl 'set -e; missing=0; for tool in g++ cmake git python3; do if ! command -v "$tool" >/dev/null 2>&1; then missing=1; fi; done; if [ "$missing" -eq 1 ]; then echo "Installing required packages with apt..."; sudo apt-get update; sudo apt-get install -y build-essential cmake git ca-certificates python3; else echo "Required WSL packages are already installed."; fi'
}

Invoke-Wsl "cmake -S . -B build -DCMAKE_BUILD_TYPE=$BuildType"
Invoke-Wsl "cmake --build build -j`$(nproc)"

if ($RunTests) {
    Invoke-Wsl "ctest --test-dir build --output-on-failure"
}

if (-not $NoRun) {
    Invoke-WslProgram @("./build/network_server_dialog")
}

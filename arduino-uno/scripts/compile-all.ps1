param(
    [string] $BuildCache = (Join-Path $env:TEMP 'qhardware-arduino-cache'),
    [string] $Fqbn = 'arduino:avr:uno'
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest

if (-not (Get-Command arduino-cli -ErrorAction SilentlyContinue)) {
    throw 'arduino-cli was not found on PATH'
}

$root = Split-Path -Parent $PSScriptRoot
$exerciseRoot = Join-Path $root 'exercises'
New-Item -ItemType Directory -Force -Path $BuildCache | Out-Null

$sketches = Get-ChildItem -LiteralPath $exerciseRoot -Filter '*.ino' -File -Recurse |
    Sort-Object FullName
if (-not $sketches) { throw "No sketches found under $exerciseRoot" }

foreach ($sketch in $sketches) {
    $exercise = Split-Path -Parent $sketch.DirectoryName
    $name = Split-Path -Leaf $exercise
    $staging = Join-Path (Join-Path $BuildCache 'staging') $name
    $output = Join-Path (Join-Path $BuildCache 'build') $name
    New-Item -ItemType Directory -Force -Path $staging, $output | Out-Null
    Copy-Item -LiteralPath $sketch.FullName -Destination (Join-Path $staging "$name.ino") -Force
    Write-Host "Compiling $name"
    & arduino-cli compile --fqbn $Fqbn --build-path $output $staging
    if ($LASTEXITCODE -ne 0) { throw "Compile failed: $name" }
}

Write-Host "Compiled $($sketches.Count) sketches."

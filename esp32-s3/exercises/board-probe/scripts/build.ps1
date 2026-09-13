param(
    [Parameter(Mandatory)] [string] $IdfPath
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
$project = Split-Path -Parent $PSScriptRoot
$export = Join-Path $IdfPath 'export.ps1'
if (-not (Test-Path -LiteralPath $export)) { throw "ESP-IDF export script not found: $export" }
. $export
Push-Location $project
try {
    idf.py set-target esp32s3
    if ($LASTEXITCODE -ne 0) { throw 'idf.py set-target failed' }
    idf.py build
    if ($LASTEXITCODE -ne 0) { throw 'idf.py build failed' }
} finally {
    Pop-Location
}

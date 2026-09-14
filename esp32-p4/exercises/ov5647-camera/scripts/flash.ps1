param(
    [Parameter(Mandatory)] [string] $IdfPath,
    [Parameter(Mandatory)] [string] $Port
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
$project = Split-Path -Parent $PSScriptRoot
$export = Join-Path $IdfPath 'export.ps1'
if (-not (Test-Path -LiteralPath $export)) { throw "ESP-IDF export script not found: $export" }
. $export
Push-Location $project
try {
    idf.py -p $Port flash
    if ($LASTEXITCODE -ne 0) { throw 'idf.py flash failed' }
} finally {
    Pop-Location
}

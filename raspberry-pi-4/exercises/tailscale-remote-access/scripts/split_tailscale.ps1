param(
    [Parameter(Mandatory)] [string] $InputPath,
    [Parameter(Mandatory)] [string] $OutputDirectory,
    [ValidateRange(1, 1024)] [int] $ChunkSizeMiB = 8
)

$ErrorActionPreference = 'Stop'
Set-StrictMode -Version Latest
$source = (Resolve-Path -LiteralPath $InputPath).Path
if (-not (Test-Path -LiteralPath $source -PathType Leaf)) { throw 'Input is not a file' }
New-Item -ItemType Directory -Force -Path $OutputDirectory | Out-Null
if (Get-ChildItem -LiteralPath $OutputDirectory -Filter 'part-*' -File) {
    throw 'Output directory already contains part files'
}

$chunkBytes = $ChunkSizeMiB * 1MB
$buffer = New-Object byte[] $chunkBytes
$input = [System.IO.File]::OpenRead($source)
try {
    $index = 0
    while (($read = $input.Read($buffer, 0, $buffer.Length)) -gt 0) {
        $part = Join-Path $OutputDirectory ('part-{0:D4}' -f $index)
        $output = [System.IO.File]::Create($part)
        try { $output.Write($buffer, 0, $read) } finally { $output.Dispose() }
        $index++
    }
} finally {
    $input.Dispose()
}

$item = Get-Item -LiteralPath $source
$hash = (Get-FileHash -Algorithm SHA256 -LiteralPath $source).Hash.ToLowerInvariant()
@(
    "filename=$($item.Name)"
    "bytes=$($item.Length)"
    "chunk_bytes=$chunkBytes"
    "parts=$index"
) | Set-Content -LiteralPath (Join-Path $OutputDirectory 'metadata.txt') -Encoding ascii
"$hash  $($item.Name)" | Set-Content -LiteralPath (Join-Path $OutputDirectory 'package.sha256') -Encoding ascii

[pscustomobject]@{ Parts = $index; Bytes = $item.Length; SHA256 = $hash }

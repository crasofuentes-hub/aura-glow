Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Fail([string]$m){ Write-Error $m; exit 1 }
function Ok([string]$m){ Write-Host $m }

# Ensure verify exists and passes
if(-not (Test-Path .\scripts\verify.ps1)){ Fail "Missing scripts/verify.ps1" }
pwsh -File .\scripts\verify.ps1 -Strict | Out-Null

# Build engine demo
if(-not (Test-Path .\engine\CMakeLists.txt)){ Fail "Missing engine/CMakeLists.txt" }

$buildDir = ".\engine\build"
cmake -S .\engine -B $buildDir | Out-Null
cmake --build $buildDir --config Release

# Run demo
$exe = Join-Path $buildDir "Release\auraglow_demo.exe"
if(-not (Test-Path $exe)){
  # fallback (some generators place exe directly)
  $exe = Join-Path $buildDir "auraglow_demo.exe"
}
if(-not (Test-Path $exe)){ Fail "Demo executable not found." }

& $exe
if($LASTEXITCODE -ne 0){ Fail "Engine demo failed." }

Ok "OK: demo ran successfully."

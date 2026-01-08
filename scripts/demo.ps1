# --- Ensure MSVC environment (auto-bootstrap) ---
if(-not (Get-Command cl.exe -ErrorAction SilentlyContinue)){
  $vsdev = "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat"
  if(Test-Path $vsdev){
    $here = (Get-Location).Path
    cmd /c "`"$vsdev`" -arch=x64 -host_arch=x64 && cd /d `"$here`" && pwsh -NoProfile -ExecutionPolicy Bypass -File `"$here\scripts\demo.ps1`""
    exit $LASTEXITCODE
  }
  throw "MSVC not found in PATH and VsDevCmd.bat not found."
}
# ----------------------------------------------
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


Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Check($name, $cmd){
  try {
    & $cmd | Out-Null
    Write-Host ("OK: " + $name)
  } catch {
    Write-Host ("MISSING/FAIL: " + $name)
  }
}

Write-Host "Aura Glow Doctor (Windows)"
Write-Host "--------------------------"

Check "git" { git --version }
Check "pwsh" { pwsh -v }
Check "cmake" { cmake --version }

# Optional checks
Check "flutter (optional)" { flutter --version }

Write-Host "Done."

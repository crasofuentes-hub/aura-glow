Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"
param([switch]$Strict)

function Fail([string]$m){ Write-Error $m; exit 1 }

$required = @("LICENSE","NOTICE","CITATION.cff","docs/LEGAL.md","docs/COMPLIANCE.md","README.md","engine/CMakeLists.txt","engine/include/auraglow.h","engine/src/auraglow.cpp","shaders/hair_dye.glsl","shaders/skin_sss.glsl","shaders/keratin.glsl","app/pubspec.yaml","app/lib/main.dart")
foreach($f in $required){
  if(-not (Test-Path $f)){ Fail ("Missing: " + $f) }
}

$lic = Get-Content "LICENSE" -Raw
if($Strict){
  if($lic.Length -lt 20000){ Fail "LICENSE too short." }
  if($lic -notmatch "GNU AFFERO GENERAL PUBLIC LICENSE"){ Fail "LICENSE header mismatch." }
}
Write-Host "OK"


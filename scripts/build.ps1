param([switch]$Test)
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path $PSScriptRoot -Parent
Push-Location $projectRoot
try {
    New-Item -ItemType Directory -Force build | Out-Null
    $sources = @('src/main.cpp')
    $target = 'build/algogenie.exe'
    if ($Test) {
        $sources = @('tests/test_main.cpp')
        $target = 'build/tests.exe'
    }
    & g++ -std=c++17 -Wall -Wextra -Wpedantic -g @sources -o $target
    if ($LASTEXITCODE -ne 0) {
        Write-Host 'If AlgoGenie is open, press Q in replay, then 0 in the main menu before rebuilding.'
        throw 'Compilation failed. Read the compiler message above.'
    }
    Write-Host "Built $target"
    if ($Test) {
        & ".\$target"
        if ($LASTEXITCODE -ne 0) { throw 'Tests failed.' }
    }
} finally { Pop-Location }

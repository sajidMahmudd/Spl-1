$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path $PSScriptRoot -Parent
Push-Location $projectRoot
try {
    $lesson = @('1','2','n','n','b','r','e','b','n','q','2','3','0') -join "`n"
    $output = ($lesson | & .\build\algogenie.exe) -join "`n"
    if ($LASTEXITCODE -ne 0) { throw 'Lesson run failed.' }
    $positions = [regex]::Matches($output, 'STEP (\d+) /') | ForEach-Object { $_.Groups[1].Value }
    if (($positions -join ',') -ne '0,1,2,1,0,23,22,23') { throw 'Replay navigation failed.' }
    if ($output -notmatch 'Output\s+\[ 1 3 3 5 8 \]') { throw 'Result-only mode failed after replay.' }
    if (([regex]::Matches($output, 'Values preserved : PASS')).Count -ne 3) { throw 'Verification labels incorrect.' }

    $invalid = @('1','1','31','0','invalid','b','e','q','0') -join "`n"
    $output = ($invalid | & .\build\algogenie.exe) -join "`n"
    if ($LASTEXITCODE -ne 0 -or $output -notmatch 'from 0 to 30' -or
        $output -notmatch 'Enter one command' -or $output -notmatch 'Values preserved : PASS') {
        throw 'Invalid input recovery / empty-array lesson failed.'
    }
    $output = ('1' | & .\build\algogenie.exe) -join "`n"
    if ($LASTEXITCODE -ne 0 -or $output -notmatch 'Session finished') { throw 'EOF handling failed.' }
    Write-Host 'PASS: replay navigation, result mode, validation, empty input and EOF.'
} finally { Pop-Location }

param([string]$Executable = '.\build\algogenie.exe')
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path $PSScriptRoot -Parent
Push-Location $projectRoot
try {
    $directory = Join-Path 'build' ('activity-tests-' + [guid]::NewGuid().ToString('N'))
    New-Item -ItemType Directory -Path $directory -Force | Out-Null
    function Run-ActivitySession([string[]]$commands) {
        $text = (($commands -join "`n") | & $Executable) -join "`n"
        if ($LASTEXITCODE -ne 0) { throw 'Application failed.' }
        return $text
    }
    foreach ($algorithm in @(1,2)) {
        foreach ($mode in @(1,2)) {
            $prefix = Join-Path $directory "a$algorithm-m$mode"
            $casePath = "$prefix-case.txt"; $csvPath = "$prefix-report.csv"; $txtPath = "$prefix-report.txt"
            $commands = @('6',"$mode","$algorithm",'2')
            if ($mode -eq 1) { $commands += @('n','b','r','e','b','n','bad','q') }
            $commands += @('4','1',$casePath,'5','1',$txtPath,'5','2',$csvPath,'0','0')
            $output = Run-ActivitySession $commands
            if ($output -notmatch 'SAVED ACTIVITY CASE:' -or $output -notmatch 'EXPORTED ACTIVITY REPORT:') { throw 'Activity persistence failed.' }
            if ($mode -eq 1) {
                $steps = @([regex]::Matches($output,'STEP (\d+) /') | ForEach-Object { [int]$_.Groups[1].Value })
                $end = if ($algorithm -eq 1) { 5 } else { 21 }
                if (($steps -join ',') -ne "0,1,0,0,$end,$($end-1),$end,$end" -or $output -notmatch 'Maximum count: PASS') {
                    throw "Activity replay navigation failed: $($steps -join ',')"
                }
            }
            $rows = @(Import-Csv -LiteralPath $csvPath)
            $expectedAlgorithm = if ($algorithm -eq 1) { 'Earliest-finish Greedy' } else { 'DP' }
            $expectedMode = if ($mode -eq 1) { 'learn' } else { 'run' }
            if ($rows.Count -ne 1 -or $rows[0].algorithm -ne $expectedAlgorithm -or $rows[0].mode -ne $expectedMode -or
                $rows[0].selected_ids -ne '1 3 4' -or $rows[0].selected_count -ne '3' -or $rows[0].median_us -ne '' -or
                $rows[0].timing_status -ne 'not_measured' -or $rows[0].feasible -ne 'true' -or $rows[0].optimal -ne 'true') { throw 'Activity CSV contents failed.' }
            $reload = Run-ActivitySession @('6','4','2',$casePath,'1',"$algorithm",'6','e','q','3','6','0','0')
            if ($reload -notmatch 'LOADED ACTIVITY CASE:' -or $reload -notmatch 'GREEDY vs DP: 3 vs 6' -or $reload -notmatch 'COMPLETE') {
                throw 'Fresh-process activity roundtrip failed.'
            }
        }
    }
    $compCsv = Join-Path $directory 'compare.csv'
    $output = Run-ActivitySession @('6','3','2','5','2',$compCsv,'0','3','4','0')
    if ($output -notmatch 'GREEDY vs DP: 3 vs 6' -or $output -notmatch 'Value comparisons\s+12\s+28') { throw 'Switching activity/sorting failed.' }
    $rows = @(Import-Csv -LiteralPath $compCsv)
    if ($rows.Count -ne 2 -or $rows[0].compatibility_checks -ne '3' -or $rows[1].compatibility_checks -ne '6' -or
        $rows[1].dp_improvements -ne '3' -or $rows[0].preparation_comparisons -ne '4' -or
        $rows[0].repetitions -ne '7' -or $rows[1].sample_7_us -eq '' -or $rows[0].trace_status -ne 'not_recorded') { throw 'Activity comparison CSV failed.' }
    $hash = (Get-FileHash -LiteralPath $compCsv).Hash
    $output = Run-ActivitySession @('6','3','2','5','2',$compCsv,'0','0')
    if ($output -notmatch 'File already exists' -or (Get-FileHash -LiteralPath $compCsv).Hash -ne $hash) { throw 'Existing report overwritten.' }
    $casePath = Join-Path $directory 'a1-m1-case.txt'
    $hash = (Get-FileHash -LiteralPath $casePath).Hash
    $output = Run-ActivitySession @('6','2','1','3','4','1',$casePath,'0','0')
    if ($output -notmatch 'File already exists' -or (Get-FileHash -LiteralPath $casePath).Hash -ne $hash) { throw 'Existing case overwritten.' }
    $bad = Join-Path $directory 'bad.txt'
    'ALGOGENIE 1 TYPE ACTIVITY COUNT 1 INTERVALS 5 2 END' | Set-Content -LiteralPath $bad -Encoding ascii
    $kept = Join-Path $directory 'kept.csv'
    $output = Run-ActivitySession @('6','3','2','4','2',$bad,'5','2',$kept,'4','2','examples/activity-empty.txt','5','0','0')
    if ($output -notmatch 'Current activity input/report unchanged' -or $output -notmatch 'No verified activity report available' -or
        @(Import-Csv -LiteralPath $kept)[0].selected_ids -ne '1 3 4') { throw 'Activity load/report state failed.' }
    $output = Run-ActivitySession @('6','5','4','1','4','2',(Join-Path $directory 'missing.txt'),'0','0')
    if ($output -notmatch 'No current activity input' -or $output -notmatch 'Cannot open that file') { throw 'Missing activity state errors failed.' }
    $large = Join-Path $directory 'large.txt'
    $pairs = (0..99 | ForEach-Object { "$_ $($_+1)" }) -join "`n"
    "ALGOGENIE 1 TYPE ACTIVITY COUNT 100 INTERVALS`n$pairs`nEND" | Set-Content -LiteralPath $large -Encoding ascii
    $output = Run-ActivitySession @('6','4','2',$large,'1','2','6','2','e','q','3','6','0','0')
    # Learn rejects the 100-item current case; choosing the small preset replaces it.
    if ($output -notmatch 'at most 20 activities' -or $output -notmatch 'Maximum count: PASS') { throw 'Activity Learn bound recovery failed.' }
    $output = Run-ActivitySession @('6','4','2',$large,'3','6','0','0')
    if ($output -notmatch 'Selected count: 100' -or $output -notmatch '99 vs 4950') { throw 'Maximum activity comparison failed.' }
    $spaceCase = Join-Path $directory 'case with spaces.txt'
    $output = Run-ActivitySession @('6','2','2','2','4','1',('"'+$spaceCase+'"'),'4','2',('"'+$spaceCase+'"'),'3','6','0','0')
    if ($output -notmatch 'LOADED ACTIVITY CASE:' -or $output -notmatch 'Matching optimal counts: PASS') { throw 'Quoted activity paths failed.' }
    $output = Run-ActivitySession @('6','1','1','1','21','1','3 3','x','-2 0','e','q','0','0')
    if ($output -notmatch 'from 0 to 20' -or $output -notmatch 'Invalid interval' -or $output -notmatch 'Maximum count: PASS') { throw 'Activity invalid custom input recovery failed.' }
    foreach ($algorithm in @(1,2)) {
        $output = Run-ActivitySession @('6','1',"$algorithm",'1','0','e','q','0','0')
        if ($output -notmatch 'Maximum selected count: 0' -or $output -notmatch 'Maximum count: PASS') { throw 'Empty activity lesson failed.' }
    }
    foreach ($preset in @(3,4,5)) {
        $output = Run-ActivitySession @('6','3',"$preset",'0','0')
        if ($output -notmatch 'Matching optimal counts: PASS') { throw 'Activity preset failed.' }
    }
    $output = Run-ActivitySession @('6','1')
    if ($output -notmatch 'Session finished') { throw 'Activity EOF handling failed.' }
    Write-Host "PASS: Activity Learn/Run/Compare, replay, file roundtrips, CSV readback, limits and recovery. Artifacts: $directory"
} finally { Pop-Location }

param([string]$Executable = '.\build\algogenie.exe')
$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path $PSScriptRoot -Parent
Push-Location $projectRoot
try {
    $testDirectory = Join-Path 'build' ('file-tests-' + [guid]::NewGuid().ToString('N'))
    New-Item -ItemType Directory -Path $testDirectory | Out-Null
    function Run-Session([string[]]$commands) {
        $text = (($commands -join "`n") | & $Executable) -join "`n"
        if ($LASTEXITCODE -ne 0) { throw 'Application exited with failure.' }
        return $text
    }
    foreach ($algorithm in @(1,2)) {
        foreach ($mode in @(1,2)) {
            $prefix = Join-Path $testDirectory "a$algorithm-m$mode"
            $casePath = "$prefix-case.txt"
            $txtPath = "$prefix-report.txt"
            $csvPath = "$prefix-report.csv"
            $commands = @("$mode","$algorithm",'3')
            if ($mode -eq 1) { $commands += @('n','b','r','e','q') }
            $commands += @('4','1',$casePath,'5','1',$txtPath,'5','2',$csvPath,'0')
            $output = Run-Session $commands
            if ($output -notmatch 'SAVED CASE:' -or $output -notmatch 'EXPORTED REPORT:') { throw 'Single-algorithm save/export failed.' }
            $rows = @(Import-Csv -LiteralPath $csvPath)
            $expectedAlgorithm = if ($algorithm -eq 1) { 'Merge Sort' } else { 'Quicksort' }
            $expectedMode = if ($mode -eq 1) { 'learn' } else { 'run' }
            if ($rows.Count -ne 1 -or $rows[0].algorithm -ne $expectedAlgorithm -or $rows[0].mode -ne $expectedMode -or
                $rows[0].input_values -ne '8 3 5 3 1' -or $rows[0].output_values -ne '1 3 3 5 8' -or
                $rows[0].timing_status -ne 'not_measured' -or $rows[0].median_us -ne '' -or
                $rows[0].sorted -ne 'true' -or $rows[0].values_preserved -ne 'true') { throw 'Single-algorithm CSV data incorrect.' }
            if ($mode -eq 1 -and ($rows[0].trace_status -ne 'complete' -or [int]$rows[0].trace_events -le 0)) {
                throw 'Learning report lacks trace metadata.'
            }
            if ((Get-Content -LiteralPath $txtPath -Raw) -notmatch 'Values preserved: PASS') { throw 'TXT report missing verification.' }
            # Reload in a fresh process and reuse through both Learn and Compare.
            $reload = Run-Session @('4','2',$casePath,'1',"$algorithm",'7','e','q','3','7','0')
            if ($reload -notmatch 'LOADED CASE:' -or $reload -notmatch 'Matching sorted outputs: PASS' -or
                $reload -notmatch 'COMPLETE') { throw 'Fresh-session roundtrip failed.' }
        }
    }
    $comparisonCsv = Join-Path $testDirectory 'comparison.csv'
    $comparisonTxt = Join-Path $testDirectory 'comparison.txt'
    $output = Run-Session @('4','2','examples/sorted.txt','3','7','5','1',$comparisonTxt,'5','2',$comparisonCsv,'0')
    $rows = @(Import-Csv -LiteralPath $comparisonCsv)
    if ($rows.Count -ne 2 -or $rows[0].algorithm -ne 'Merge Sort' -or $rows[1].algorithm -ne 'Quicksort' -or
        $rows[0].comparisons -ne '12' -or $rows[1].comparisons -ne '28' -or
        $rows[0].repetitions -ne '7' -or $rows[1].warmups -ne '1' -or $rows[0].sample_7_us -eq '' -or
        $rows[0].timing_method -notmatch 'steady_clock' -or $rows[0].build_id -eq '') { throw 'Comparison CSV schema/data failed.' }
    if ((Get-Content -LiteralPath $comparisonTxt -Raw) -notmatch 'Matching sorted outputs: PASS') { throw 'Comparison TXT failed.' }
    $oldHash = (Get-FileHash -LiteralPath $comparisonCsv).Hash
    $output = Run-Session @('3','2','5','2',$comparisonCsv,'0')
    if ($output -notmatch 'File already exists' -or (Get-FileHash -LiteralPath $comparisonCsv).Hash -ne $oldHash) {
        throw 'Existing report was overwritten.'
    }
    $casePath = Join-Path $testDirectory 'a1-m1-case.txt'
    $oldHash = (Get-FileHash -LiteralPath $casePath).Hash
    $output = Run-Session @('2','1','2','4','1',$casePath,'0')
    if ($output -notmatch 'File already exists' -or (Get-FileHash -LiteralPath $casePath).Hash -ne $oldHash) {
        throw 'Existing case was overwritten.'
    }
    $bad = Join-Path $testDirectory 'bad.txt'
    'ALGOGENIE 1 TYPE ARRAY COUNT 3 VALUES 1 2 END' | Set-Content -LiteralPath $bad -Encoding ascii
    $output = Run-Session @('3','2','4','2',$bad,'3','7','4','2','examples/empty.txt','5','0')
    if ($output -notmatch 'Current input/report unchanged' -or $output -notmatch 'Value comparisons\s+3\s+2' -or
        $output -notmatch 'No verified report available') { throw 'Case/report state isolation failed.' }
    $output = Run-Session @('5','4','1','4','2',(Join-Path $testDirectory 'missing.txt'),'0')
    if ($output -notmatch 'No verified report' -or $output -notmatch 'No current input' -or
        $output -notmatch 'Cannot open that file') { throw 'Empty-session/missing-file errors failed.' }
    $hundred = Join-Path $testDirectory 'hundred.txt'
    "ALGOGENIE 1 TYPE ARRAY COUNT 100 VALUES $((1..100) -join ' ') END" | Set-Content -LiteralPath $hundred -Encoding ascii
    $output = Run-Session @('4','2',$hundred,'1','1','7','2','e','q','0')
    if ($output -notmatch 'at most 30 elements' -or $output -notmatch 'COMPLETE') { throw 'Loaded Learn limit recovery failed.' }
    $spacePath = Join-Path $testDirectory 'case with spaces.txt'
    $output = Run-Session @('2','2','2','4','1',('"' + $spacePath + '"'),'4','2',('"' + $spacePath + '"'),'3','7','0')
    if ($output -notmatch 'LOADED CASE:' -or $output -notmatch 'Matching sorted outputs: PASS') { throw 'Quoted paths with spaces failed.' }
    $output = Run-Session @('2','1','2','5','1',(Join-Path $testDirectory 'no-folder/report.txt'),'4','1','wrong.csv','','0')
    if ($output -notmatch 'Cannot create the report' -or $output -notmatch 'filename ending in .txt') { throw 'Write/path errors failed.' }
    foreach ($example in @('examples/empty.txt','examples/negative.txt','examples/duplicates.txt','examples/small.txt')) {
        $output = Run-Session @('4','2',$example,'3','7','0')
        if ($output -notmatch 'Matching sorted outputs: PASS') { throw "Example failed: $example" }
    }
    Write-Host "PASS: Learn/Run/Compare file workflows for both algorithms; CSV readback; error recovery. Artifacts: $testDirectory"
} finally { Pop-Location }

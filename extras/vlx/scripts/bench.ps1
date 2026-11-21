<#
.SYNOPSIS
  Compare VLX with other VLC builds on this PC: install size, start-up time, memory and CPU while playing.

.DESCRIPTION
  Each app is a folder holding vlx.exe or vlc.exe (a portable copy). The script adds an empty "portable" folder next to
  the exe so every run uses a throwaway profile beside it, never %APPDATA%\vlc. It runs the apps one at a time.

  Start-up = launch to the main window existing, median of the warm runs (the first run builds the plugin
  cache and is reported separately). Memory = private bytes and working set after 10 s idle, and the peak
  working set while playing. CPU = share of all cores, averaged over the playback window after a warm-up.

.EXAMPLE
  .\bench.ps1 -App "VLC 3.0.24=T:\dev\vlx-bench\vlc-3.0.24", "VLX=T:\dev\vlx-bench\vlx" -Sample T:\dev\vlx-bench\bbb-1080p.mp4
#>
param(
    [Parameter(Mandatory)] [string[]] $App,       # "Name=folder with vlx.exe or vlc.exe"
    [Parameter(Mandatory)] [string]   $Sample,    # a video file
    [int] $StartupRuns = 6,
    [int] $PlaySeconds = 60,
    [int] $WarmupSeconds = 5,
    [string] $Out = "bench-results"
)
$ErrorActionPreference = 'Stop'
New-Item -ItemType Directory -Force $Out | Out-Null
$cores = [Environment]::ProcessorCount
$common = @('--no-qt-privacy-ask')  # the only first-run option both VLC 3 and 4 know

function Get-VlcVersion($exe) {
    (Get-Item $exe).VersionInfo.ProductVersion
}

function Start-Vlc($exe, [string[]] $args2) {
    $psi = New-Object System.Diagnostics.ProcessStartInfo $exe
    $psi.Arguments = ($common + $args2 | ForEach-Object { if ($_ -match '\s') { '"' + $_ + '"' } else { $_ } }) -join ' '
    $psi.UseShellExecute = $false
    $psi.WorkingDirectory = Split-Path $exe
    [System.Diagnostics.Process]::Start($psi)
}

function Stop-Vlc($p) {
    if (-not $p.HasExited) { $p.CloseMainWindow() | Out-Null; if (-not $p.WaitForExit(5000)) { $p.Kill(); $p.WaitForExit() } }
}

function Measure-Startup($exe) {
    $sw = [System.Diagnostics.Stopwatch]::StartNew()
    $p = Start-Vlc $exe @()
    while ($p.MainWindowHandle -eq 0 -and $sw.Elapsed.TotalSeconds -lt 60) { Start-Sleep -Milliseconds 20; $p.Refresh() }
    $ms = $sw.Elapsed.TotalMilliseconds
    Start-Sleep -Milliseconds 500
    Stop-Vlc $p
    [math]::Round($ms)
}

$rows = @()
foreach ($spec in $App) {
    $name, $dir = $spec -split '=', 2
    # VLX builds are vlx.exe; VLC (and VLX builds before 0.1.0) are vlc.exe
    $exe = @('vlx.exe', 'vlc.exe') | ForEach-Object { Join-Path $dir $_ } | Where-Object { Test-Path $_ } | Select-Object -First 1
    if (-not $exe) { throw "No vlx.exe or vlc.exe in $dir" }
    $portable = Join-Path $dir 'portable'
    if (Test-Path $portable) { Remove-Item -Recurse -Force $portable }
    New-Item -ItemType Directory $portable | Out-Null
    Write-Host "== $name ($(Get-VlcVersion $exe))"

    # Install size
    $files = Get-ChildItem -Recurse -File $dir | Where-Object { $_.FullName -notlike "$portable*" }
    $sizeMB = [math]::Round(($files | Measure-Object Length -Sum).Sum / 1MB, 1)
    $sizeNoLocaleMB = [math]::Round(($files | Where-Object { $_.FullName -notlike '*\locale\*' } | Measure-Object Length -Sum).Sum / 1MB, 1)
    $plugins = ($files | Where-Object { $_.FullName -like '*\plugins\*' -and $_.Extension -eq '.dll' }).Count

    # Start-up
    $times = 1..$StartupRuns | ForEach-Object { Measure-Startup $exe }
    $first = $times[0]
    $warm = ($times | Select-Object -Skip 1 | Sort-Object)[[math]::Floor(($StartupRuns - 1) / 2)]

    # Idle memory
    $p = Start-Vlc $exe @()
    Start-Sleep -Seconds 10; $p.Refresh()
    $idlePrivate = [math]::Round($p.PrivateMemorySize64 / 1MB, 1)
    $idleWs = [math]::Round($p.WorkingSet64 / 1MB, 1)
    Stop-Vlc $p

    # Playback
    $p = Start-Vlc $exe @('--loop', $Sample)
    Start-Sleep -Seconds $WarmupSeconds; $p.Refresh()
    $cpu0 = $p.TotalProcessorTime; $t0 = Get-Date; $peakWs = 0
    while (((Get-Date) - $t0).TotalSeconds -lt $PlaySeconds) {
        Start-Sleep -Milliseconds 500; $p.Refresh()
        if ($p.WorkingSet64 -gt $peakWs) { $peakWs = $p.WorkingSet64 }
    }
    $p.Refresh()
    $cpuPct = [math]::Round((($p.TotalProcessorTime - $cpu0).TotalSeconds / ((Get-Date) - $t0).TotalSeconds / $cores) * 100, 1)
    $playPrivate = [math]::Round($p.PrivateMemorySize64 / 1MB, 1)
    Stop-Vlc $p
    Remove-Item -Recurse -Force $portable

    $rows += [pscustomobject]@{
        App = $name; Version = Get-VlcVersion $exe
        'Install size (MB)' = $sizeMB; 'Without translations (MB)' = $sizeNoLocaleMB; Files = $files.Count; Plugins = $plugins
        'Start-up, first (ms)' = $first; 'Start-up, warm median (ms)' = $warm
        'Idle private (MB)' = $idlePrivate; 'Idle working set (MB)' = $idleWs
        'Playing private (MB)' = $playPrivate; 'Playing peak working set (MB)' = [math]::Round($peakWs / 1MB, 1)
        'Playing CPU (% of all cores)' = $cpuPct
    }
}

$stamp = Get-Date -Format 'yyyy-MM-dd HHmm'
$rows | Export-Csv -NoTypeInformation (Join-Path $Out "bench-$($stamp -replace ' ','-').csv")

# Markdown table: one row per metric, one column per app
$metrics = $rows[0].PSObject.Properties.Name | Where-Object { $_ -ne 'App' }
$md = @("Benchmark, $stamp, $env:COMPUTERNAME, $cores logical cores. Sample: $(Split-Path $Sample -Leaf), $PlaySeconds s of playback.", '')
$md += '| Metric | ' + (($rows | ForEach-Object App) -join ' | ') + ' |'
$md += '|---|' + (($rows | ForEach-Object { '---:' }) -join '|') + '|'
foreach ($m in $metrics) { $md += "| $m | " + (($rows | ForEach-Object { $_.$m }) -join ' | ') + ' |' }
$md | Set-Content -Encoding utf8 (Join-Path $Out "bench-$($stamp -replace ' ','-').md")
$md -join "`n"

# 計測シナリオを段階1〜3で繰り返し実行し、CSVを bench/results に保存する
# 使い方: powershell -ExecutionPolicy Bypass -File bench\run_bench.ps1 [-Runs 11] [-Stages 1,2,3]
# run=0 は各段階の初回(コールド)として集計時に分けて扱う
param(
    [int]$Runs = 11,
    [int[]]$Stages = @(1, 2, 3),
    [int]$TimeoutSec = 300
)

$root = Split-Path -Parent $PSScriptRoot   # リポジトリのルート
$exe = Join-Path $root "x64\Release\DirectX12.exe"
$workDir = Join-Path $root "DirectX12"   # assets/ への相対パスの基準
$outDir = Join-Path $root "bench\results"
New-Item -ItemType Directory -Force $outDir | Out-Null

if (-not (Test-Path $exe)) { throw "Release ビルドが見つかりません: $exe" }

# 段階間の時間的なゆらぎを均すため、run ごとに段階を順に回す
for ($run = 0; $run -lt $Runs; $run++) {
    foreach ($stage in $Stages) {
        $csv = Join-Path $outDir ("stage{0}_run{1:D2}.csv" -f $stage, $run)
        $argList = @("--bench", "--stage=$stage", "--run=$run", "--out=$csv")
        Write-Host ("stage {0} run {1} ..." -f $stage, $run) -NoNewline
        $p = Start-Process -FilePath $exe -ArgumentList $argList -WorkingDirectory $workDir -PassThru
        if (-not $p.WaitForExit($TimeoutSec * 1000)) {
            $p.Kill()
            Write-Host " timeout"
            continue
        }
        if (Test-Path $csv) { Write-Host (" ok (exit {0})" -f $p.ExitCode) }
        else { Write-Host (" FAILED (exit {0}, CSVなし)" -f $p.ExitCode) }
    }
}

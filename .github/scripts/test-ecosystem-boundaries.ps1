param(
    [Parameter(Mandatory = $true)]
    [string]$ContractsSource,

    [Parameter(Mandatory = $true)]
    [string]$FrameworksRoot
)

$ErrorActionPreference = 'Stop'

$contracts = (Resolve-Path -LiteralPath $ContractsSource).Path
$frameworks = (Resolve-Path -LiteralPath $FrameworksRoot).Path

$modules = @(
    @{ Name = 'MCF'; Path = $contracts; Allowed = @('vosp/contracts') },
    @{ Name = 'MProtocolF'; Path = (Join-Path $frameworks 'MicroProtocolFramework');
       Allowed = @('vosp/protocol', 'vosp/contracts') },
    @{ Name = 'MEF'; Path = (Join-Path $frameworks 'MicroErrorFramework');
       Allowed = @('vosp.hpp', 'vosp/error', 'vosp/logger', 'vosp/version',
                   'vosp/worker_pool', 'vosp/contracts') },
    @{ Name = 'MPF'; Path = (Join-Path $frameworks 'MicroPersistenceFramework');
       Allowed = @('vosp/persistence', 'vosp/contracts') },
    @{ Name = 'MTF'; Path = (Join-Path $frameworks 'MicroTelemetryFramework');
       Allowed = @('vosp/telemetry', 'vosp/contracts') },
    @{ Name = 'MConfigF'; Path = (Join-Path $frameworks 'MicroConfigurationFramework');
       Allowed = @('vosp/configuration', 'vosp/contracts') },
    @{ Name = 'MRF'; Path = (Join-Path $frameworks 'MicroResilienceFramework');
       Allowed = @('vosp/resilience', 'vosp/contracts') },
    @{ Name = 'MWF'; Path = (Join-Path $frameworks 'MicroWorkflowFramework');
       Allowed = @('vosp/workflow', 'vosp/contracts') },
    @{ Name = 'MSF'; Path = (Join-Path $frameworks 'MicroServiceFramework');
       Allowed = @('vosp/service', 'vosp/contracts') }
)

$violations = [System.Collections.Generic.List[string]]::new()
$includePattern = '^\s*#\s*include\s*[<"](?<path>vosp(?:\.hpp|/[^>"]+))[>"]'

foreach ($module in $modules) {
    foreach ($directoryName in @('include', 'src')) {
        $directory = Join-Path $module.Path $directoryName
        if (-not (Test-Path -LiteralPath $directory)) {
            continue
        }

        $files = Get-ChildItem -LiteralPath $directory -Recurse -File |
            Where-Object { $_.Extension -in @('.h', '.hpp', '.cpp', '.cc', '.cxx') }
        foreach ($file in $files) {
            $lineNumber = 0
            foreach ($line in Get-Content -LiteralPath $file.FullName) {
                ++$lineNumber
                if ($line -notmatch $includePattern) {
                    continue
                }

                $include = $Matches.path
                $allowed = $false
                foreach ($prefix in $module.Allowed) {
                    if ($include.StartsWith($prefix, [System.StringComparison]::Ordinal)) {
                        $allowed = $true
                        break
                    }
                }
                if (-not $allowed) {
                    $relative = [System.IO.Path]::GetRelativePath($module.Path, $file.FullName)
                    $violations.Add("$($module.Name): ${relative}:${lineNumber} includes <$include>")
                }
            }
        }
    }
}

$mcfBuildFiles = @((Join-Path $contracts 'CMakeLists.txt')) +
    @(Get-ChildItem -LiteralPath (Join-Path $contracts 'cmake') -Recurse -File -ErrorAction SilentlyContinue |
      ForEach-Object { $_.FullName })
$runtimeDependencyPattern =
    'Micro(?:Error|Protocol|Persistence|Telemetry|Configuration|Resilience|Workflow|Service)Framework|vosp::(?:vosp|protocol|persistence|telemetry|configuration|resilience|workflow|service)'
foreach ($match in Select-String -Path $mcfBuildFiles -Pattern $runtimeDependencyPattern) {
    $relative = [System.IO.Path]::GetRelativePath($contracts, $match.Path)
    $violations.Add("MCF build graph: ${relative}:$($match.LineNumber) references runtime code")
}

if ($violations.Count -ne 0) {
    $violations | ForEach-Object { Write-Error $_ }
    throw "Ecosystem dependency direction has $($violations.Count) violation(s)"
}

Write-Output "Ecosystem dependency direction verified for $($modules.Count) frameworks"

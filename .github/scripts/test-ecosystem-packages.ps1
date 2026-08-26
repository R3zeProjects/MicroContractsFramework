param(
    [Parameter(Mandatory = $true)]
    [string]$ContractsSource,

    [Parameter(Mandatory = $true)]
    [string]$FrameworksRoot,

    [Parameter(Mandatory = $true)]
    [string]$BuildRoot,

    [string]$CxxCompiler = ''
)

$ErrorActionPreference = 'Stop'

$contracts = (Resolve-Path -LiteralPath $ContractsSource).Path
$frameworks = (Resolve-Path -LiteralPath $FrameworksRoot).Path
$build = [System.IO.Path]::GetFullPath($BuildRoot)
$prefix = Join-Path $build 'install'
$compilerOption = if ($CxxCompiler) {
    @("-DCMAKE_CXX_COMPILER=$CxxCompiler")
} else {
    @()
}

function Invoke-CMake {
    param([Parameter(ValueFromRemainingArguments = $true)][string[]]$Arguments)

    & cmake @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "CMake failed with exit code ${LASTEXITCODE}: $($Arguments -join ' ')"
    }
}

function Install-Framework {
    param(
        [string]$Name,
        [string]$Source,
        [string[]]$Options = @()
    )

    $frameworkBuild = Join-Path $build $Name
    $configure = @(
        '-S', $Source,
        '-B', $frameworkBuild,
        '-DCMAKE_BUILD_TYPE=Release',
        '-DBUILD_TESTING=OFF',
        "-DCMAKE_INSTALL_PREFIX=$prefix",
        "-DCMAKE_PREFIX_PATH=$prefix"
    ) + $compilerOption + $Options

    Invoke-CMake @configure
    Invoke-CMake --build $frameworkBuild --config Release --parallel
    Invoke-CMake --install $frameworkBuild --config Release
}

Install-Framework 'mcf' $contracts @('-DMCF_BUILD_EXAMPLES=OFF')
Install-Framework 'mef' (Join-Path $frameworks 'MicroErrorFramework') @(
    '-DMEF_BUILD_EXAMPLES=OFF', '-DMEF_FETCH_CONTRACTS=OFF')
Install-Framework 'mpf' (Join-Path $frameworks 'MicroPersistenceFramework') @(
    '-DMPF_BUILD_EXAMPLES=OFF', '-DMPF_FETCH_CONTRACTS=OFF')
Install-Framework 'mtf' (Join-Path $frameworks 'MicroTelemetryFramework') @(
    '-DMTF_BUILD_EXAMPLES=OFF', '-DMTF_FETCH_CONTRACTS=OFF')
Install-Framework 'mconfig' (Join-Path $frameworks 'MicroConfigurationFramework') @(
    '-DMCONFIG_BUILD_EXAMPLES=OFF', '-DMCONFIG_FETCH_CONTRACTS=OFF')
Install-Framework 'mrf' (Join-Path $frameworks 'MicroResilienceFramework') @(
    '-DMRF_BUILD_EXAMPLES=OFF', '-DMRF_FETCH_CONTRACTS=OFF')
Install-Framework 'mwf' (Join-Path $frameworks 'MicroWorkflowFramework') @(
    '-DMWF_BUILD_EXAMPLES=OFF', '-DMWF_FETCH_CONTRACTS=OFF')
Install-Framework 'msf' (Join-Path $frameworks 'MicroServiceFramework') @(
    '-DMSF_BUILD_EXAMPLES=OFF', '-DMSF_FETCH_CONTRACTS=OFF')

$consumerBuild = Join-Path $build 'consumer'
$consumerConfigure = @(
    '-S', (Join-Path $contracts 'tests/ecosystem_consumer'),
    '-B', $consumerBuild,
    '-DCMAKE_BUILD_TYPE=Release',
    "-DCMAKE_PREFIX_PATH=$prefix"
) + $compilerOption
Invoke-CMake @consumerConfigure
Invoke-CMake --build $consumerBuild --config Release --parallel
& ctest --test-dir $consumerBuild -C Release --output-on-failure
if ($LASTEXITCODE -ne 0) {
    throw "Ecosystem package consumer failed with exit code $LASTEXITCODE"
}

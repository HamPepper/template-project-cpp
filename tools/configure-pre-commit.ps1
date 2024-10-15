$scriptDir = Split-Path -Path $MyInvocation.MyCommand.Definition -Parent
$parentDir = Split-Path -Path $scriptDir -Parent

$parentDirFwdSlashes = $parentDir -replace '\\', '/'

$preCommitJson = @"
repos:
- repo: local
  hooks:
    - id: clang-format
      name: clang-format
      always_run: false
      args: []
      entry: ${parentDirFwdSlashes}/tools/clang-format-18.exe -style=file -i
      exclude: '^$'
      exclude_types: []
      fail_fast: false
      language: system
      files: ''
      types_or:
        - c++
- repo: https://github.com/editorconfig-checker/editorconfig-checker.python
  rev: 3.0.3
  hooks:
    - id: editorconfig-checker
      alias: ec
"@


# configure PATH
function AddTo-EnvVar{
param(
    [string]$Dir,
    [string]$Var
)
    # NOTE: for now don't check if the dir exists or not.
    # if( !(Test-Path $Dir) ){
    #     Write-warning "Supplied directory was not found!"
    #     return
    # }

    $value = [Environment]::GetEnvironmentVariable($Var, "User")
    if( $value -notlike "*"+$Dir+"*" ){
        [Environment]::SetEnvironmentVariable($Var, "$Dir;$value", "User")
    }
}


# install pre-commit
$wingetPath = -join([Environment]::GetEnvironmentVariable("LOCALAPPDATA"), "\Microsoft\WinGet\Links")
$uvExe = -join($wingetPath, "\uv.exe")
& $uvExe tool install pre-commit

$uvToolPath = -join([Environment]::GetEnvironmentVariable("USERPROFILE"), "\.local\bin")
AddTo-EnvVar $uvToolPath "PATH"


# deploy pre-commit config
$preCommitConfigFile = -join($parentDir, "\.pre-commit-config.yaml")
Set-Content -Path $preCommitConfigFile -Value $preCommitJson


# install pre-commit hooks
Set-Location -Path $parentDir
$preCommitExe = -join($uvToolPath, "\pre-commit.exe")
& $preCommitExe
& $preCommitExe install
& $preCommitExe run --all

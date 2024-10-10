# set workdir
$scriptDir = Split-Path -Path $MyInvocation.MyCommand.Definition -Parent
$parentDir = Split-Path -Path $scriptDir -Parent
Set-Location -Path $parentDir

# use ninjia to generate compile_commands.json
cmake --preset compdb

# use compdb to post-process compile_commands.json
uv run compdb -p .\build_compdb\ list > compile_commands.json

# replace 'build_compdb' with 'build' in compile_commands.json
(Get-Content compile_commands.json) -replace 'build_compdb', 'build' | Set-Content compile_commands.json

function WinGet-Install {
param(
    [string]$Package
)
    Write-Output "install $Package..."
    winget install -e --id $Package -s winget `
        --no-upgrade `
        --silent `
        --accept-package-agreements `
        --accept-source-agreements
}

function WinGet-Install-All {
param(
    [string[]]$Packages
)
    foreach ($pkg in $Packages) {
        WinGet-Install $pkg
    }
}


$pkgs = @(
    "Kitware.CMake"
    "DimitriVanHeesch.Doxygen"
    #"Graphviz.Graphviz"  # can't generate SVG graph, at least on github action
)

WinGet-Install-All $pkgs


# msvc
winget install -e --id Microsoft.VisualStudio.2022.BuildTools `
    -s winget `
    --no-upgrade `
    --silent `
    --accept-package-agreements `
    --accept-source-agreements `
    --override '--passive --wait --add Microsoft.VisualStudio.Workload.VCTools;includeRecommended'

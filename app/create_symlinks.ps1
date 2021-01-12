cd $PSScriptRoot

function New-Symlink ($LinkName, $TargetName) {
    $link = ".\{0}" -f $LinkName
    $target = "..\solution\{0}" -f $TargetName
    New-Item -Path $link -ItemType SymbolicLink -Value $target
}

New-Symlink -LinkName "local_debug" -TargetName "Debug"
New-Symlink -LinkName "local_release" -TargetName "Release"

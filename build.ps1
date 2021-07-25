Push-Location .

$build_dir = 'build'

if (-Not (Test-Path -Path $build_dir)) {
  mkdir $build_dir
}

.\format.ps1

Set-Location $build_dir

Write-Output ""
Write-Output ""
Write-Output "~~~~~~~~~~~~ Starting debug build ~~~~~~~~~~~~"
Write-Output ""

if ($IsLinux) {
  mkdir Debug
  Set-Location Debug
  cmake -DCMAKE_BUILD_TYPE=Debug ../..  
} else {
  cmake -DCMAKE_BUILD_TYPE=Debug ..
}

cmake --build . --config Debug

if (-Not ($LASTEXITCODE -eq "0")) {
  Write-Output "cmake --build for Debug mode failed!"
  Pop-Location
  exit 1
}

Write-Output ""
Write-Output "~~~~~~~~~~~~ Completed debug build ~~~~~~~~~~~~"
Write-Output ""
Write-Output ""

Pop-Location
Push-Location .
Set-Location $build_dir

Write-Output ""
Write-Output ""
Write-Output "~~~~~~~~~~~~ Starting release build ~~~~~~~~~~~~"
Write-Output ""

if ($IsLinux) {
  mkdir Release
  Set-Location Release
  cmake -DCMAKE_BUILD_TYPE=Release ../..
} else {
  cmake -DCMAKE_BUILD_TYPE=Release ..
}

cmake --build . --config Release

if (-Not ($LASTEXITCODE -eq "0")) {
  Write-Output "cmake --build for Release mode failed!"
  Pop-Location
  exit 1
}

Write-Output ""
Write-Output "~~~~~~~~~~~~ Completed release build ~~~~~~~~~~~~"
Write-Output ""
Write-Output ""

Pop-Location
exit 0

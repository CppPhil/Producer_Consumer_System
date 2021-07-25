Push-Location .

$build_dir = 'build'

if (-Not (Test-Path -Path $build_dir)) {
  mkdir $build_dir
}

Set-Location .\external\pthreads

if ($IsWindows) {
  & cmd.exe /c "`"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat`" && nmake VC install"

  if (-Not ($LASTEXITCODE -eq "0")) {
    Write-Output "Could not run nmake!"
    Pop-Location
    exit 1
  }
}

Set-Location .\..\..

if (Test-Path -Path "$build_dir\PTHREADS-BUILT") {
  & cmd.exe /c "del /s /f /q `"$build_dir\PTHREADS-BUILT`""
  & cmd.exe /c "rmdir /s /q `"$build_dir\PTHREADS-BUILT`""
}

Move-Item .\external\PTHREADS-BUILT "$build_dir\PTHREADS-BUILT"

.\format.ps1

Set-Location $build_dir

Write-Output ""
Write-Output ""
Write-Output "~~~~~~~~~~~~ Starting debug build ~~~~~~~~~~~~"
Write-Output ""

cmake -DCMAKE_BUILD_TYPE=Debug -G "Visual Studio 16 2019" -A Win32 ..
cmake --build . --config Debug

if (-Not ($LASTEXITCODE -eq "0")) {
  Write-Output "cmake --build for Debug mode failed!"
  Pop-Location
  exit 1
}

Pop-Location
Push-Location .

Set-Location $build_dir
Copy-Item -Path ".\PTHREADS-BUILT\bin\pthreadVC3-w64.dll" -Destination ".\Debug\pthreadVC3-w64.dll"

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

cmake -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 16 2019" -A Win32 ..
cmake --build . --config Release

if (-Not ($LASTEXITCODE -eq "0")) {
  Write-Output "cmake --build for Release mode failed!"
  Pop-Location
  exit 1
}

Pop-Location
Push-Location .

Set-Location $build_dir
Copy-Item -Path ".\PTHREADS-BUILT\bin\pthreadVC3-w64.dll" -Destination ".\Release\pthreadVC3-w64.dll"

Write-Output ""
Write-Output "~~~~~~~~~~~~ Completed release build ~~~~~~~~~~~~"
Write-Output ""
Write-Output ""

Pop-Location
exit 0

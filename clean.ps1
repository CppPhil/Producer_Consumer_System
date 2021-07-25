Push-Location .

$build_dir = 'build'

if (-Not (Test-Path -Path $build_dir)) {
  mkdir $build_dir
}

if (Test-Path -Path "$build_dir\PTHREADS-BUILT") {
  & cmd.exe /c "del /s /f /q `"$build_dir\PTHREADS-BUILT`""
  & cmd.exe /c "rmdir /s /q `"$build_dir\PTHREADS-BUILT`""
}

if (Test-Path -Path "external\PTHREADS-BUILT") {
  & cmd.exe /c "del /s /f /q `"external\PTHREADS-BUILT`""
  & cmd.exe /c "rmdir /s /q `"external\PTHREADS-BUILT`""
}

Pop-Location
exit 0

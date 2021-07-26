Push-Location .

$build_dir = 'build'

if (-Not (Test-Path -Path $build_dir)) {
  mkdir $build_dir
}

& .\$build_dir\Debug\consumer_producer_app.exe @args

if (-Not ($LASTEXITCODE -eq "0")) {
  Write-Output ".\Debug\consumer_producer_app.exe exited with a non-zero exit code!"
  Pop-Location
  exit 1
}

Pop-Location
exit 0

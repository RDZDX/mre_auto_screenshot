"C:\Program Files\MRE_SDK\tools\DllPackage.exe" "D:\MyGitHub\mre_auto_screenshot\mre_auto_screenshot.vcproj"
if %errorlevel% == 0 (
 echo postbuild OK.
  copy mre_auto_screenshot.vpp ..\..\..\MoDIS_VC9\WIN32FS\DRIVE_E\mre_auto_screenshot.vpp /y
exit 0
)else (
echo postbuild error
  echo error code: %errorlevel%
  exit 1
)


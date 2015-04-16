@echo off

rem call vcvarsall.bat

for %%f in (*_test.cpp) do call :test %%f
exit /B %ERRORLEVEL%

:test
  echo %1 ...
  cl -EHsc -MT -nologo -D_CRT_SECURE_NO_DEPRECATE -DWIN32 %1
  "%~n1.exe"
exit /B %ERRORLEVEL%

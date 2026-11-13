rem ---------------------------------------------------------------------------

set PATH=%ARMGCC_DIR%\bin;%PATH%

set TOP_DIR=.
set OBJ_DIR=%TOP_DIR%\obj
set ELF_DIR=%TOP_DIR%\bin

mkdir %OBJ_DIR%
mkdir %ELF_DIR%
make -f make_app_os_simple TOP_DIR=%TOP_DIR% OBJ_DIR=%OBJ_DIR% ELF_DIR=%ELF_DIR%

pause
exit

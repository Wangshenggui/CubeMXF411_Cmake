@echo off
taskkill /f /im arm-none-eabi-gdb.exe >nul 2>&1
taskkill /f /im gdb.exe >nul 2>&1

@REM cmake --preset=Debug
@REM cmake -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE="../cmake/gcc-arm-none-eabi.cmake" ..
@REM openocd -f interface/stlink.cfg -f target/stm32f4x.cfg
arm-none-eabi-gdb -q -ex "file build/F411_Cmake.elf" -ex "target remote localhost:3333" -ex "load" -ex "continue"

exit
@echo off
setlocal EnableDelayedExpansion

:: =========================================================================
:: build.bat - Windows Build script for simd-f128
:: =========================================================================

:parse_args
if "%~1"=="" goto interactive_menu
if /i "%~1"=="-h" goto show_help
if /i "%~1"=="--help" goto show_help
set TARGET=%~1
goto dispatch_target

:show_help
echo Usage: build.bat [target]
echo.
echo Targets:
echo   tests      Build and run unit tests (Default)
echo   bench      Build and run benchmarks
echo   examples   Build example applications
echo   asan       Build and run tests with AddressSanitizer
echo   check      Check system dependencies
echo   clean      Remove all build directories
echo   all        Build tests, benchmarks, and examples
echo.
exit /b 0

:dispatch_target
if /i "%TARGET%"=="tests" goto build_tests
if /i "%TARGET%"=="bench" goto build_bench
if /i "%TARGET%"=="examples" goto build_examples
if /i "%TARGET%"=="asan" goto build_asan
if /i "%TARGET%"=="check" goto check_environment
if /i "%TARGET%"=="clean" goto build_clean
if /i "%TARGET%"=="all" goto build_all
echo Unknown target: %TARGET%
goto show_help

:check_environment
echo ====================================================================================
echo  Checking Windows build environment and dependencies...
echo ====================================================================================

set MISSING_CRITICAL=0

:: 1. check for cmake
where cmake >nul 2>nul
if %errorlevel% equ 0 (
    for /f "tokens=*" %%i in ('cmake --version') do (
        echo  [OK] cmake found: %%i
        goto :cmake_done
    )
    :cmake_done
    echo. >nul
) else (
    echo  [FAIL] cmake is NOT installed or not found in PATH!
    set MISSING_CRITICAL=1
)

:: 2. check for c/c++ compiler
set COMPILER_FOUND=0
where cl >nul 2>nul
if %errorlevel% equ 0 (
    echo  [OK] C compiler found: MSVC (cl.exe)
    set COMPILER_FOUND=1
) else (
    where gcc >nul 2>nul
    if !errorlevel! equ 0 (
        echo  [OK] C compiler found: MinGW GCC (gcc.exe)
        set COMPILER_FOUND=1
    ) else (
        where clang >nul 2>nul
        if !errorlevel! equ 0 (
            echo  [OK] C compiler found: LLVM Clang (clang.exe)
            set COMPILER_FOUND=1
        )
    )
)

if %COMPILER_FOUND% equ 0 (
    echo  [FAIL] No C/C++ compiler (cl.exe, gcc.exe, clang.exe) was found in PATH!
    set MISSING_CRITICAL=1
)

echo ====================================================================================
if %MISSING_CRITICAL% neq 0 (
    echo  ERROR: Critical build tools are missing. Please install them before continuing.
    echo ====================================================================================
    exit /b 1
) else (
    echo  System environment check passed!
    echo ====================================================================================
    exit /b 0
)

:build_tests
call :check_environment || exit /b 1
echo =^> Building Tests...
cmake -S . -B build_tests -DCMAKE_BUILD_TYPE=Release
cmake --build build_tests --config Release --parallel
echo =^> Running Tests...
cd build_tests && ctest -C Release --output-on-failure
cd ..
exit /b %errorlevel%

:build_bench
call :check_environment || exit /b 1
echo =^> Building Benchmarks...
cmake -S . -B build_bench -DCMAKE_BUILD_TYPE=Release -DSIMD_F128_BUILD_BENCHMARKS=ON
cmake --build build_bench --config Release --parallel
echo =^> Running Benchmarks...
if exist build_bench\benchmarks\Release\bench_compare.exe (
    build_bench\benchmarks\Release\bench_compare.exe
    echo.
)
build_bench\benchmarks\Release\bench_arithmetic.exe
build_bench\benchmarks\Release\bench_math.exe
build_bench\benchmarks\Release\bench_matrix.exe
exit /b %errorlevel%

:build_examples
call :check_environment || exit /b 1
echo =^> Building Examples...
cmake -S . -B build_examples -DCMAKE_BUILD_TYPE=Release -DSIMD_F128_BUILD_EXAMPLES=ON
cmake --build build_examples --config Release --parallel
echo =^> Examples built in build_examples\examples\Release\
exit /b %errorlevel%

:build_asan
call :check_environment || exit /b 1
echo =^> Building Tests with ASAN...
cmake -S . -B build_asan -DCMAKE_BUILD_TYPE=Debug -DSIMD_F128_ENABLE_SANITY=ON
cmake --build build_asan --config Debug --parallel
echo =^> Running ASAN Tests...
cd build_asan && ctest -C Debug --output-on-failure
cd ..
exit /b %errorlevel%

:build_clean
echo =^> Cleaning build directories...
if exist build_tests rmdir /s /q build_tests
if exist build_bench rmdir /s /q build_bench
if exist build_examples rmdir /s /q build_examples
if exist build_asan rmdir /s /q build_asan
if exist build_wasm rmdir /s /q build_wasm
if exist build rmdir /s /q build
echo Done.
exit /b 0

:build_all
call :build_tests
call :build_bench
call :build_examples
exit /b 0

:interactive_menu
echo =========================================================================
echo  simd-f128 Build Menu (Windows)
echo =========================================================================
echo   1) Run Tests (Default)
echo   2) Run Benchmarks
echo   3) Build Examples
echo   4) Run Tests with AddressSanitizer (ASAN)
echo   5) Clean Build Directories
echo   6) Check Environment
echo   q) Quit
echo =========================================================================
set /p CHOICE="Select an option [1-6,q]: "

if "%CHOICE%"=="" goto build_tests
if "%CHOICE%"=="1" goto build_tests
if "%CHOICE%"=="2" goto build_bench
if "%CHOICE%"=="3" goto build_examples
if "%CHOICE%"=="4" goto build_asan
if "%CHOICE%"=="5" goto build_clean
if "%CHOICE%"=="6" goto check_environment
if /i "%CHOICE%"=="q" exit /b 0

echo Invalid option.
exit /b 1

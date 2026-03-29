@echo off
g++ -std=c++17 -Wall -Iinclude src/*.cpp "./test/Bruteforce.cpp" -o bruteforce_detector.exe
if %errorlevel% equ 0 (
    echo Build Successful! Running program...
    bruteforce_detector.exe
) else (
    echo Build Failed. Check errors above.
)
pause
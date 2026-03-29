@echo off
g++ -std=c++17 -Wall -Iinclude src/*.cpp "./test/Minhash LSH.cpp" -o near_dup_detector.exe
if %errorlevel% equ 0 (
    echo Build Successful! Running program...
    near_dup_detector.exe
) else (
    echo Build Failed. Check errors above.
)
pause
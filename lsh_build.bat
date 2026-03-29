@echo off
g++ -std=c++17 -Wall -Iinclude src/*.cpp "./test/Minhash LSH.cpp" -o minhash_lsh_detector.exe
if %errorlevel% equ 0 (
    echo Build Successful! Running program...
    minhash_lsh_detector.exe
) else (
    echo Build Failed. Check errors above.
)
pause
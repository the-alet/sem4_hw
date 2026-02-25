@echo off
setlocal enabledelayedexpansion

set "actual=test_actual"
set "expected=test_expected"
set "logfile=testlog.txt"

set "files=res01u.txt res01d.txt res01l.txt res01r.txt res02u.txt res02d.txt res02l.txt res02r.txt res03u.txt res03d.txt res03l.txt res03r.txt res04u.txt res04d.txt res04l.txt res04r.txt res05.txt res06.txt res07.txt res08.txt res09.txt res10.txt"
type nul > "%logfile%"

if not exist "%actual%\" (
    echo Error: folder "%actual%" not found. >> "%logfile%"
    exit /b 1
)
if not exist "%expected%\" (
    echo Error: folder "%expected%" not found. >> "%logfile%"
    exit /b 1
)

set "diff_count=0"
set "missing_count=0"

echo Comparing specified files in "%actual%" and "%expected%"... >> "%logfile%"
echo. >> "%logfile%"

for %%f in (%files%) do (
    set "file=%%f"
    set "actual_file=%actual%\!file!"
    set "expected_file=%expected%\!file!"
    
    if exist "!actual_file!" (
        if exist "!expected_file!" (
            echo. >> "%logfile%"
            echo Comparing !file! ... >> "%logfile%"
            fc /L "!actual_file!" "!expected_file!" >> "%logfile%" 2>&1
            if errorlevel 1 (
                echo Result: !file! - DIFFERS >> "%logfile%"
                set /a diff_count+=1
            ) else (
                echo Result: !file! - identical >> "%logfile%"
            )
        ) else (
            echo File !file! present in "%actual%" but missing in "%expected%" >> "%logfile%"
            set /a missing_count+=1
        )
    ) else (
        if exist "!expected_file!" (
            echo File !file! present in "%expected%" but missing in "%actual%" >> "%logfile%"
            set /a missing_count+=1
        ) else (
            echo File !file! missing in both folders >> "%logfile%"
            set /a missing_count+=1
        )
    )
)

echo. >> "%logfile%"
echo Summary: differences: %diff_count%, missing: %missing_count% >> "%logfile%"

if %diff_count% neq 0 exit /b 1
if %missing_count% neq 0 exit /b 1
exit /b 0
#!/bin/bash
actual="../test_actual"
expected="../test_expected"
logfile="testlog.txt"
files="res01u.txt res01d.txt res01l.txt res01r.txt res02u.txt res02d.txt res02l.txt res02r.txt res03u.txt res03d.txt res03l.txt res03r.txt res04u.txt res04d.txt res04l.txt res04r.txt res05.txt res06.txt res07.txt res08.txt res09.txt res10.txt"
> "$logfile"
if [ ! -d "$actual" ]; then
    echo "Error: folder '$actual' not found."
    exit 1
fi
if [ ! -d "$expected" ]; then
    echo "Error: folder '$expected' not found." >> "$logfile"
    echo "Error: folder '$expected' not found."
    exit 1
fi
diff_count=0
missing_count=0
echo "Comparing specified files in '$actual' and '$expected'..." >> "$logfile"
echo >> "$logfile"
for file in $files; do
    actual_file="$actual/$file"
    expected_file="$expected/$file"
    if [ -f "$actual_file" ]; then
        if [ -f "$expected_file" ]; then
            echo >> "$logfile"
            echo "Comparing $file ..." >> "$logfile"
            if diff -q --strip-trailing-cr "$actual_file" "$expected_file" >> "$logfile" 2>&1; then
                echo "Result: $file - identical" >> "$logfile"
            else
                echo "Result: $file - DIFFERS" >> "$logfile"
                diff_count=$((diff_count + 1))
            fi
        else
            echo "File $file present in '$actual' but missing in '$expected'" >> "$logfile"
            missing_count=$((missing_count + 1))
        fi
    else
        if [ -f "$expected_file" ]; then
            echo "File $file present in '$expected' but missing in '$actual'" >> "$logfile"
            missing_count=$((missing_count + 1))
        else
            echo "File $file missing in both folders" >> "$logfile"
            missing_count=$((missing_count + 1))
        fi
    fi
done
echo >> "$logfile"
echo "Summary: differences: $diff_count, missing: $missing_count" >> "$logfile"
echo "Summary: differences: $diff_count, missing: $missing_count"
if [ $diff_count -ne 0 ] || [ $missing_count -ne 0 ]; then
    exit 1
fi
exit 0
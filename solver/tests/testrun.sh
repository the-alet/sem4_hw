#!/bin/bash
mkdir -p ../test_actual
SOLVER="../solver"
$SOLVER ../test_input/test01u.txt ../test_actual/res01u.txt
$SOLVER ../test_input/test01d.txt ../test_actual/res01d.txt
$SOLVER ../test_input/test01r.txt ../test_actual/res01r.txt
$SOLVER ../test_input/test01l.txt ../test_actual/res01l.txt
$SOLVER ../test_input/test02u.txt ../test_actual/res02u.txt
$SOLVER ../test_input/test02d.txt ../test_actual/res02d.txt
$SOLVER ../test_input/test02r.txt ../test_actual/res02r.txt
$SOLVER ../test_input/test02l.txt ../test_actual/res02l.txt
$SOLVER ../test_input/test03u.txt ../test_actual/res03u.txt
$SOLVER ../test_input/test03d.txt ../test_actual/res03d.txt
$SOLVER ../test_input/test03r.txt ../test_actual/res03r.txt
$SOLVER ../test_input/test03l.txt ../test_actual/res03l.txt
$SOLVER ../test_input/test04u.txt ../test_actual/res04u.txt
$SOLVER ../test_input/test04d.txt ../test_actual/res04d.txt
$SOLVER ../test_input/test04r.txt ../test_actual/res04r.txt
$SOLVER ../test_input/test04l.txt ../test_actual/res04l.txt
$SOLVER ../test_input/test05.txt ../test_actual/res05.txt
$SOLVER ../test_input/test06.txt ../test_actual/res06.txt
$SOLVER ../test_input/test07.txt ../test_actual/res07.txt
$SOLVER ../test_input/test08.txt ../test_actual/res08.txt
$SOLVER ../test_input/test09.txt ../test_actual/res09.txt
$SOLVER ../test_input/test10.txt ../test_actual/res10.txt

echo "All tests passed"
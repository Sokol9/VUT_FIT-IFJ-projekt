#!/usr/bin/env bash
current=$(pwd -P)
tests=0
vTests=0
itests=0


echo "=====TESTING of VALID TESTS===="

for file in "$current"/valid_tests/* ; do
	echo "***"
	echo "TESTING FILE: $file"
	./compiler <"$file"
	retVal=$?

	((tests=$tests+1))
	if [ $retVal -eq 0 ]; then 
		((vTests=$vTests+1))
		echo "valid test"
		echo""
	else
		((iTests=$iTests+1))
		echo "invalid test, with error: $retVal"
		echo""
	fi
done

echo "================================="
echo "===========TESTING END==========="
echo "Total tests: $tests"
echo "Passed tests: $vTests"
echo "Failed tests: $iTests"



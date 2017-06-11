#!/bin/bash
FAIL=0

# Make a fresh copy of the target program
make clean
make
if [ $? -ne 0 ]; then
  echo "**** Make (compilation) FAILED"
  exit 13
fi

# Compare the content of two given filenames, fail if they differ or
# if one of them doesn't exist.  The first filename should be the
# expected file, and the second should be the one created by the
# program.  An optional third parameter says what to call the file
# that doesn't match.
matchFile() {
    if [ "$#" -eq 3 ]; then
	name="$3"
    else
	name="$2"
    fi

    if [ ! -f "$1" ]; then
	echo "   **** Something's wrong.  Expected a file named $1."
	FAIL=1
	return 1
    fi

    if [ -f "$1" ] && [ ! -f "$2" ]; then
	echo "   **** Test failed - file $2 doesn't exist, but it should."
	FAIL=1
	return 1
    fi

    
    if ! diff -q "$1" "$2" >/dev/null; then
	echo "   **** Test failed - $name doesn't match expected"
	FAIL=1
	return 1
    fi

    return 0
}


# Function to run the program against a test case, checking
# its output, exit status and error output against what's expected.
runtest() {
    TESTNO="$1"
    shift
    EX_STATUS="$1"
    shift

    # Remove any files that we want to test for.
    rm -f output.txt stderr.txt
    rm -f text_$TESTNO.txt.bak

    # get a clean copy of the text file.
    if [ -f orig_$TESTNO.txt ]; then
	cp orig_$TESTNO.txt text_$TESTNO.txt
    fi

    # Run the program differently, depending on whether or not
    # we're using an alternate dictionary file.
    if [ $# -gt 0 ]
    then
	DFILE="$1"
	shift
	
	echo "Test $TESTNO: ./spellcheck text_$TESTNO.txt $DFILE < input_$TESTNO.txt > output.txt 2> stderr.txt"
	./spellcheck text_$TESTNO.txt $DFILE < input_$TESTNO.txt > output.txt 2> stderr.txt
	STATUS=$?
    else
	echo "Test $TESTNO: ./spellcheck text_$TESTNO.txt < input_$TESTNO.txt > output.txt 2> stderr.txt"
	./spellcheck text_$TESTNO.txt < input_$TESTNO.txt > output.txt 2> stderr.txt
	STATUS=$?
    fi
      

    # Make sure we got the expected exit status.
    if [ $STATUS -ne $EX_STATUS ]; then
	echo "   **** Test failed - incorrect exit status. Expected: $EX_STATUS Got: $STATUS"
	FAIL=1
	return 1
    fi

    if [ $EX_STATUS -eq 0 ]; then
	# Make sure the program's output looks right.
	if ! matchFile eoutput_$TESTNO.txt output.txt "standard output"; then
	    return 1
	fi

	# Make sure the resulting text file looks right.
	if ! matchFile etext_$TESTNO.txt text_$TESTNO.txt "text file text_$TESTNO.txt"; then
	    return 1
	fi

	# Make sure we got a backup of the text file.
	if ! matchFile orig_$TESTNO.txt text_$TESTNO.txt.bak; then
	    return 1
	fi

	if [ -s stderr.txt ]; then
	    echo "   **** Test failed - shouldn't be any output on standard error"
	    FAIL=1
	    return 1
	fi
    else
	# Make sure the program's standard output looks right.
	if ! matchFile eoutput_$TESTNO.txt output.txt "standard output"; then
	    return 1
	fi

	# Make sure the program's error output looks right.
	if ! matchFile estderr_$TESTNO.txt stderr.txt "error output"; then
	    return 1
	fi

	# Shouldn't have changed the original text file
	if [ -f orig_$TESTNO.txt ] && ! matchFile orig_$TESTNO.txt text_$TESTNO.txt; then
	    return 1
	fi
    fi
    
    echo "Test $TESTNO passed"
    return 0
}

# Run each of the test cases
runtest 01 0
runtest 02 0
runtest 03 1
runtest 04 0
runtest 05 0
runtest 06 0 bigwords.txt
runtest 07 0
runtest 08 1 badwords_1.txt
runtest 09 1 badwords_2.txt
runtest 10 1 badwords_3.txt
runtest 11 0
runtest 12 1 bigwords.txt
runtest 13 0
runtest 14 1
runtest 15 1 missing.txt

# Bad command-line arguments as one last test.
echo "Test 16: ./spellcheck > output.txt 2> stderr.txt"
./spellcheck > output.txt 2> stderr.txt
STATUS=$?

# Make sure we got the expected exit status.
if [ $STATUS -ne 1 ]; then
    echo "   **** Test failed - incorrect exit status. Expected: $EX_STATUS Got: $STATUS"
    FAIL=1
else
    if matchFile eoutput_$TESTNO.txt output.txt "standard output"; then
	echo "Test 16 passed"
    fi
fi

if [ $FAIL -ne 0 ]; then
  echo "FAILING TESTS!"
  exit 13
fi

exit 0

# Test file for "Lab6-SynchSM"


# commands.gdb provides the following functions for ease:
#   test "<message>"
#       Where <message> is the message to print. Must call this at the beginning of every test
#       Example: test "PINA: 0x00 => expect PORTC: 0x01"
#   checkResult
#       Verify if the test passed or failed. Prints "passed." or "failed." accordingly, 
#       Must call this at the end of every test.
#   expectPORTx <val>
#       With x as the port (A,B,C,D)
#       The value the port is epected to have. If not it will print the erroneous actual value
#   setPINx <val>
#       With x as the port or pin (A,B,C,D)
#       The value to set the pin to (can be decimal or hexidecimal
#       Example: setPINA 0x01
#   printPORTx f OR printPINx f 
#       With x as the port or pin (A,B,C,D)
#       With f as a format option which can be: [d] decimal, [x] hexadecmial (default), [t] binary 
#       Example: printPORTC d
#   printDDRx
#       With x as the DDR (A,B,C,D)
#       Example: printDDRB

echo ======================================================\n
echo Running all tests..."\n\n

# light behavior, no button press
test "PINA 0x00, init, on1, on2, on3,  => PORTB: 4"
setPINA ~0x00
set state = init
continue 10
expect state on1
continue 10
expect state on2
continue 10
expect state on3
expectPORTB 4
checkResult

#light behavior, button press on state 2 (light B1)
test "PINA 0x00, init, on1, on2, PINA 0x01, press,PINA 0x00 wait 60 ticks => PORTB = 2"
setPINA ~0x00
set state = init
continue 10
expect state on1
continue 10
expect state on2
setPINA ~0x01
continue 1
expect state press
expectPORTB 2
setPINA ~0x00
continue 60
expectPORTB 2
expect state wait
checkResult

#light behavior, button press on state 2 (light B1) then reset game and continue 
test "Test 2 but press button and reset game: ...wait 60 ticks PINA 0x01 init on1 => PORB = 1"
setPINA ~0x00
set state = init
continue 10
expect state on1
continue 10
expect state on2
setPINA ~0x01
continue 1
expect state press
expectPORTB 2
setPINA ~0x00
continue 60
expectPORTB 2
expect state wait
setPINA ~0x01
continue 1
setPINA ~0x00
continue 10
expect state on1
expectPORTB 1
checkResult

# Add tests below

# Report on how many tests passed/tests ran
set $passed=$tests-$failed
eval "shell echo Passed %d/%d tests.\n",$passed,$tests
echo ======================================================\n

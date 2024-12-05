.text
.globl main

main:
    // Clear the mcycle CSR by writing 0 to it
    csrw mcycle, x0
    // Call the game function
    jal game
    // Read mcycle into a0 (argument a0)
    csrr a0, mcycle
    // Insert code to print out the value of t0 to know how long time foo consumed
    jal print_dec

print_dec:
    li	t0, a0			# environment call with t0 = a0 will print out
	ecall				# one byte from a0 to the Run I/O window
// Clear the mcycle CSR by writing 0 to it
csrw mcycle, x0
// Call the foo function
jal foo
// Read mcycle into a0 (argument a0)
csrr a0, mcycle
// Insert code to print out the value of t0 to know how long time foo consumed
jal print_dec

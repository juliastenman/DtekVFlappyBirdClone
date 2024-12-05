void print(char *s);  // Add this at the beginning of your file
int foo_time; // Create a variable called foo_time
// Clear the mcycle CSR by writing 0 to it
asm volatile ("csrw mcycle, x0");
// Call the foo function
foo();
// Read the mcycle value into foo_time
asm("csrr %0, mcycle" : "=r"(foo_time) );
// Print out the value of foo_time (requires print_dec in time4riscv.zip)
print("\nTime for foo() was: ");
print_dec(foo_time);
print("\n");
void print(char*);
void print_dec(unsigned int);
void game(int);
void labinit(void);

int x; // Create a variable called x

void clear_registers() {
    // clear lower 32 btis
    // Clear the mcycle CSR by writing 0 to it
    asm volatile ("csrw mcycle, x0");
    asm volatile ("csrw minstret, x0");
    asm volatile ("csrw mhpmcounter3, x0");
    asm volatile ("csrw mhpmcounter4, x0");
    asm volatile ("csrw mhpmcounter5, x0");
    asm volatile ("csrw mhpmcounter6, x0");
    asm volatile ("csrw mhpmcounter7, x0");
    asm volatile ("csrw mhpmcounter8, x0");
    asm volatile ("csrw mhpmcounter9, x0");
    
    // clear higher 32 bits
    // Clear the mcycle CSR by writing 0 to it
    asm volatile ("csrw mcycleh, x0");
    asm volatile ("csrw minstreth, x0");
    asm volatile ("csrw mhpmcounter3h, x0");
    asm volatile ("csrw mhpmcounter4h, x0");
    asm volatile ("csrw mhpmcounter5h, x0");
    asm volatile ("csrw mhpmcounter6h, x0");
    asm volatile ("csrw mhpmcounter7h, x0");
    asm volatile ("csrw mhpmcounter8h, x0");
    asm volatile ("csrw mhpmcounter9h, x0");
}

void display_registers() {
    
    // Display lower 32 bits
    print("Lower 32 bits: \n");
    // Read the mcycle value into x
    asm("csrr %0, mcycle" : "=r"(x) );
    // Print out the value of foo_time (requires print_dec in time4riscv.zip)
    print("\nNumber of cycles for game() was: ");
    print_dec(x);
    print("\n");
    
    asm("csrr %0, minstret" : "=r"(x) );
    print("\nNumber of retired instructions for game() was: ");
    print_dec(x);
    print("\n");
    
    asm("csrr %0, mhpmcounter3" : "=r"(x) );
    print("\nNumber of retired memory instructions for game() was: ");
    print_dec(x);
    print("\n");
    
    asm("csrr %0, mhpmcounter4" : "=r"(x) );
    print("\nI-cache miss from instruction-fetch for game() was: ");
    print_dec(x);
    print("\n");
    
    asm("csrr %0, mhpmcounter5" : "=r"(x) );
    print("\nD-cache miss from memory operation for game() was: ");
    print_dec(x);
    print("\n");
    
    asm("csrr %0, mhpmcounter6" : "=r"(x) );
    print("\nNumber of stall from I-cache misses in CPU for game() was: ");
    print_dec(x);
    print("\n");
    
    asm("csrr %0, mhpmcounter7" : "=r"(x) );
    print("\nNumber of stall from D-cache misses in CPU for game() was: ");
    print_dec(x);
    print("\n");
    
    asm("csrr %0, mhpmcounter8" : "=r"(x) );
    print("\nStalls due to data hazards that cannot be solved by forwarding in the CPU for game() was: ");
    print_dec(x);
    print("\n");
    
    asm("csrr %0, mhpmcounter9" : "=r"(x) );
    print("\nNumber of stalls due to expensive ALU operations in CPU for game() was: ");
    print_dec(x);
    print("\n");

    print("---------------------------\n");
 
    // Print higher 32 bits
    print("Higher 32 bits: \n");
    // Read the mcycle value into x
    asm("csrr %0, mcycleh" : "=r"(x) );
    // Print out the value of foo_time (requires print_dec in time4riscv.zip)
    print("\nNumber of cycles for game() was: ");
    print_dec(x);
    print("\n");
    
    asm("csrr %0, minstreth" : "=r"(x) );
    print("\nNumber of retired instructions for game() was: ");
    print_dec(x);
    print("\n");
    
    asm("csrr %0, mhpmcounter3h" : "=r"(x) );
    print("\nNumber of retired memory instructions for game() was: ");
    print_dec(x);
    print("\n");
    
    asm("csrr %0, mhpmcounter4h" : "=r"(x) );
    print("\nI-cache miss from instruction-fetch for game() was: ");
    print_dec(x);
    print("\n");
    
    asm("csrr %0, mhpmcounter5h" : "=r"(x) );
    print("\nD-cache miss from memory operation for game() was: ");
    print_dec(x);
    print("\n");
    
    asm("csrr %0, mhpmcounter6h" : "=r"(x) );
    print("\nNumber of stall from I-cache misses in CPU for game() was: ");
    print_dec(x);
    print("\n");
    
    asm("csrr %0, mhpmcounter7h" : "=r"(x) );
    print("\nNumber of stall from D-cache misses in CPU for game() was: ");
    print_dec(x);
    print("\n");
    
    asm("csrr %0, mhpmcounter8h" : "=r"(x) );
    print("\nStalls due to data hazards that cannot be solved by forwarding in the CPU for game() was: ");
    print_dec(x);
    print("\n");
    
    asm("csrr %0, mhpmcounter9h" : "=r"(x) );
    print("\nNumber of stalls due to expensive ALU operations in CPU for game() was: ");
    print_dec(x);
    print("\n");
}

int main(){
    labinit();
    clear_registers();

    // Call the game function
    game(0);
    
    display_registers();
}
void print(char*);
void print_dec(unsigned int);
void game(void);

int main(){
    int game_time; // Create a variable called game_time
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
    // Call the game function
    game();
    // Read the mcycle value into game_time
    asm("csrr %0, mcycle" : "=r"(game_time) );
    asm("csrr %0, minstret" : "=r"(game_time) );
    asm("csrr %0, mhpmcounter3" : "=r"(game_time) );
    asm("csrr %0, mhpmcounter4" : "=r"(game_time) );
    asm("csrr %0, mhpmcounter5" : "=r"(game_time) );
    asm("csrr %0, mhpmcounter6" : "=r"(game_time) );
    asm("csrr %0, mhpmcounter7" : "=r"(game_time) );
    asm("csrr %0, mhpmcounter8" : "=r"(game_time) );
    asm("csrr %0, mhpmcounter9" : "=r"(game_time) );

    // Print out the value of foo_time (requires print_dec in time4riscv.zip)
    print("\nTime for game() was: ");
    print_dec(game_time);
    print("\n");
}
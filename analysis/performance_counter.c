void print(char*);
void print_dec(unsigned int);
void game(void);

int main(){
    int game_time; // Create a variable called foo_time
    // Clear the mcycle CSR by writing 0 to it
    asm volatile ("csrw mcycle, x0");
    // Call the game function
    game();
    // Read the mcycle value into foo_time
    asm("csrr %0, mcycle" : "=r"(game_time) );
    // Print out the value of foo_time (requires print_dec in time4riscv.zip)
    print("\nTime for game() was: ");
    print_dec(game_time);
    print("\n");
}
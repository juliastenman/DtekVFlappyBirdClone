/* Below functions can be found in other files. */
extern void enable_interrupt(void);
extern void display_string(string);


volatile char *VGA = (volatile char*) 0x08000000;
volatile int *VGA_CTRL = (volatile int*) 0x04000100;

volatile int *button_ptr = (unsigned int*) 0x040000d0;

// screen size
const int WIDTH = 320;
const int HEIGHT = 240;
const int SIZE = WIDTH*HEIGHT;

const int SKY_HEIGHT = 200;

// colors
const char BGR_COLOR = 50;
const char GROUND_COLOR = 20;
const char BIRD_COLOR = 0b11100000;

// bird pos.
const int BIRD_POS_X = WIDTH/5;
const int BIRD_START_POS_Y = HEIGHT/2;
const int BIRD_RADIUS = 15;

// speeds
const int FALL_SPEED = 1;	// pixels down per frame
const int JUMP_SPEED = 5; 	// pixels up per frame
const int JUMP_TIME = 5; 	// frames for each jump

// is modified when jump
volatile int jump_frames = 0;


void handle_interrupt(int mcause) {
	// button press
	if (mcause == 18) {
		jump_frames = JUMP_TIME;

		// button_ptr + 3 is the edge capture register 
		// (setting the bit for the button clears the edge capture)
		*(button_ptr+3) |= 0b1; 
		
	} else if (mcause == 16) {
		return;
	}

}

void labinit(void) {
	// button_ptr + 2 is the interrupt mask 
	// setting bit 1 allows button to interrupt
	*(button_ptr+2) = 0b1; 
	enable_interrupt();
}

// void testVGA() {
// 	// vertical scrolling
// 	for(int i = 0; i< 320*480; i++) {
// 		VGA[i] = i/320;
// 	}

// 	unsigned int y_ofs = 5;
// 	while(1) {
// 		*(VGA_CTRL+1) = (unsigned int) (VGA+y_ofs*320);
// 		*(VGA_CTRL+0) = 0;
// 		y_ofs = (y_ofs+1) % 240;
// 		for (int i = 0; i < 1000000; i++) {
// 			asm volatile ("nop");
// 		}

// 	}

// }

void drawBackground(int VGA_offset) {
		// Sky
		for (int i=0; i<WIDTH*SKY_HEIGHT;i++) {
			*(VGA+VGA_offset*SIZE+i) = BGR_COLOR;
		}

		// Ground
		for (int i=WIDTH*SKY_HEIGHT; i<SIZE;i++) {
			*(VGA+VGA_offset*SIZE+i) = GROUND_COLOR;
		}	
}

void drawBird(int bird_pos_y, int VGA_offset) {
		// Bird, uses (BIRD_POS_X, bird_pos_y) as center
		for (int y=bird_pos_y-BIRD_RADIUS; y<bird_pos_y+BIRD_RADIUS;y++) {
			for (int x=BIRD_POS_X-BIRD_RADIUS; x<BIRD_POS_X+BIRD_RADIUS;x++) {
				*(VGA + VGA_offset*SIZE + y*WIDTH + x) = BIRD_COLOR;
			}
		}
}

void game() {
	// offset for writing to first or second half of VGA buffer
	int VGA_offset = 0;
	int bird_pos_y = BIRD_START_POS_Y;

	while (1) {
		drawBackground(VGA_offset);
		drawBird(bird_pos_y, VGA_offset);


		// sends vga address as output
		*(VGA_CTRL+1) = (unsigned int) VGA+VGA_offset*SIZE;
		
		// find out what this does
		*(VGA_CTRL+0) = 0;

		// Small delay
		for (int i = 0; i < 1000000; i++) {
			asm volatile("nop");
		}

		// swap side of vga buffer		
		VGA_offset = (VGA_offset+1)%2;

		// bird's movement
		if (jump_frames > 0) {
			display_string("jump frames is > 0");
			bird_pos_y -= JUMP_SPEED;
			jump_frames--;
		} else {
			// display_string("falling");
			bird_pos_y += FALL_SPEED;
		}


		// positive y means downwards
		if (bird_pos_y + BIRD_RADIUS >= SKY_HEIGHT){
			break;
		}
	}
}

// for VGA 
int main(){
	labinit();
	game();
}
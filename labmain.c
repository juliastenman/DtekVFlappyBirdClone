/* Below functions can be found in other files. */
extern void enable_interrupt(void);
extern void display_string(char*);
extern char* time2string(int time);

volatile char *VGA = (volatile char*) 0x08000000;
volatile int *VGA_CTRL = (volatile int*) 0x04000100;

volatile int *button_ptr = (volatile int*) 0x040000d0;

// screen size
const int WIDTH = 320;
const int HEIGHT = 240;
const int SIZE = WIDTH*HEIGHT;

const int SKY_HEIGHT = 200;

// colors
const char BGR_COLOR = 50;
const char GROUND_COLOR = 20;
const char BIRD_COLOR = 0b11100000;
const char PIPE_COLOR = 0;//0b00011100;

// bird pos.
const int BIRD_POS_X = WIDTH/5;
const int BIRD_START_POS_Y = HEIGHT/2;
const int BIRD_RADIUS = 10;

// pipes
const int PIPE_RADIUS=15;
const int PIPE_HORIZONTAL_SPACE = 130;
const int PIPE_HALF_GAP = 30;
const int PIPE_MIN_HEIGHT = 50;
const int MAX_PIPES = WIDTH / PIPE_HORIZONTAL_SPACE + 1;
const int PIPES_TO_GENERATE = 500;

// speeds
const int PIPE_SPEED = 3;
const int FALL_SPEED = 3;	// pixels down per frame
const int JUMP_SPEED = 6; 	// pixels up per frame
const int JUMP_TIME = 2; 	// frames for each jump

// is modified when jump
int jump_frames = 0;

// stores all pipes
int pipes[500];

void setup_pipes() {
	for (int i=0; i<500;i++) {
		pipes[i] = (11111/i) % (SKY_HEIGHT - 2*PIPE_MIN_HEIGHT) + PIPE_MIN_HEIGHT;
	}
}


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

/*
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
*/

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

// Bird, uses (BIRD_POS_X, bird_pos_y) as center
void drawBird(int bird_pos_y, int VGA_offset) {
	// highest position and lowest allowed y value on screen is 0 
	int y0=bird_pos_y-BIRD_RADIUS > 0 ? bird_pos_y-BIRD_RADIUS : 0;
	for (int y=y0; y<bird_pos_y+BIRD_RADIUS;y++) {
		for (int x=BIRD_POS_X-BIRD_RADIUS; x<BIRD_POS_X+BIRD_RADIUS;x++) {
			*(VGA + VGA_offset*SIZE + y*WIDTH + x) = BIRD_COLOR;
		}
	}
}


void drawPipes(int *pipe_idx, int *first_pipe_center_x, int VGA_offset) {
	// update first pipe if out of frame
	if (*first_pipe_center_x < 0 - PIPE_RADIUS) {
		++(*pipe_idx);
		*first_pipe_center_x += PIPE_HORIZONTAL_SPACE;
	}
	int idx = *pipe_idx;
	int x_center = *first_pipe_center_x;
	
	while (x_center < WIDTH + PIPE_RADIUS) {
		int x0 = x_center-PIPE_RADIUS > 0 ? x_center-PIPE_RADIUS : 0;
		int x1 = x_center+PIPE_RADIUS < WIDTH ? x_center+PIPE_RADIUS : WIDTH;
		
		int y_center = pipes[idx];

		// high part of pipe
		for (int y=0; y<y_center-PIPE_HALF_GAP; y++) {
			for (int x=x0; x<x1; x++) {
				*(VGA + VGA_offset*SIZE + y*WIDTH + x) = PIPE_COLOR;
			}
		}

		// low part of pipe
		for (int y=y_center+PIPE_HALF_GAP; y<SKY_HEIGHT; y++) {
			for (int x=x0; x<x1; x++) {
				*(VGA + VGA_offset*SIZE + y*WIDTH + x) = PIPE_COLOR;
			}
		}

		++idx;
		x_center += PIPE_HORIZONTAL_SPACE;
	}
	*first_pipe_center_x -= PIPE_SPEED;
}

void game() {
	// offset for writing to first or second half of VGA buffer
	int VGA_offset = 0;
	int bird_pos_y = BIRD_START_POS_Y;
	int pipe_idx = 0;
	int first_pipe_center_x = WIDTH + PIPE_RADIUS;
	setup_pipes();
	while (1) {
		drawBackground(VGA_offset);
		drawBird(bird_pos_y, VGA_offset);
		drawPipes(&pipe_idx, &first_pipe_center_x, VGA_offset);	


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


		// positive y means downwards
		if (bird_pos_y + BIRD_RADIUS >= SKY_HEIGHT){
			break;
		}

		// if bird could collide with pipe on x-axis
		if (BIRD_POS_X+BIRD_RADIUS>=first_pipe_center_x-PIPE_RADIUS && BIRD_POS_X-BIRD_RADIUS<=first_pipe_center_x+PIPE_RADIUS) {
			if (bird_pos_y+BIRD_RADIUS>=pipes[pipe_idx]+PIPE_HALF_GAP || bird_pos_y-BIRD_RADIUS<=pipes[pipe_idx]-PIPE_HALF_GAP) {
				break;
			} 
		}

// 		bird's movement
		if (jump_frames > 0) {
			// display_string("jump frames is > 0\n");
			bird_pos_y -= JUMP_SPEED;
			jump_frames--;
		} else {
			// display_string("falling");
			bird_pos_y += FALL_SPEED;
		}
	}
}

// for VGA 
int main(){
	labinit();
	game();
}

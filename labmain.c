#define PIPES_TO_GENERATE 1000
// check that code above works

/* Below functions can be found in other files. */
extern void enable_interrupt(void);
extern void display_string(char*);
extern char* time2string(int time);

volatile char *VGA = (volatile char*) 0x08000000;
volatile int *VGA_CTRL = (volatile int*) 0x04000100;

volatile int *button_ptr = (volatile int*) 0x040000d0;
volatile unsigned char *display_ptr = (unsigned char*) 0x04000050;      // 1 byte per display
volatile unsigned int *switch_ptr = (unsigned int*) 0x04000010;     // 10 bits (2 bytes sufficient)

// screen size
const int WIDTH = 320;
const int HEIGHT = 240;
const int SIZE = WIDTH*HEIGHT;

const int SKY_HEIGHT = 200;

const int PLAY_WIDTH = 70;

// colors
const char BGR_COLOR = 50;
const char GROUND_COLOR = 20;
const char BIRD_COLOR = 0b11100000;
const char PIPE_COLOR = 0;//0b00011100;

// bird pos.
const int BIRD_POS_X = WIDTH/5;
const int BIRD_START_POS_Y = HEIGHT/2;
const int BIRD_RADIUS = 7;

// pipes
const int PIPE_RADIUS=15;
const int PIPE_HORIZONTAL_SPACE = 130;
const int PIPE_HALF_GAP = 30;
const int PIPE_MIN_HEIGHT = 50;
// const int MAX_PIPES = WIDTH / PIPE_HORIZONTAL_SPACE + 1;
// const int PIPES_TO_GENERATE = 500;

// speeds
// const float FRAME_SPEED = 10;
const int PIPE_SPEED = 3;
const int FALL_SPEED = 3;	// pixels down per frame
const int JUMP_SPEED = 15; 	// pixels up per frame
const int JUMP_TIME = 1; 	// frames for each jump

// is modified when jump
int jump_frames = 0;

int HIGH_SCORE_START = 0;
volatile int* high_score = &HIGH_SCORE_START;

// might have to change back PIPES_TO_GENERATE to constant 500
// stores all pipes
int pipes[PIPES_TO_GENERATE];

int toggle_high_score_val = 0;
volatile int* toggle_high_score = &toggle_high_score_val;

// true if in game
int in_main_menu_val = 0;
volatile int* in_main_menu = &in_main_menu_val;

// true if in game
int in_game_val = 0;
volatile int* in_game = &in_game_val;

// true if proceed to next screen
int proceed_val = 0;
volatile int* proceed = &proceed_val;

void setup_pipes() {
	for (int i=0; i<PIPES_TO_GENERATE;i++) {
		pipes[i] = ((11111+7*i)/(i%23)) % (SKY_HEIGHT - 2*PIPE_MIN_HEIGHT) + PIPE_MIN_HEIGHT;
	}
}

void setup_bg_ground() {
	for (int VGA_offset=0; VGA_offset<2;VGA_offset++) {
		// Ground
		for (int i=WIDTH*SKY_HEIGHT; i<SIZE;i++) {
			*(VGA+VGA_offset*SIZE+i) = GROUND_COLOR;
		}	
	}
}

/* Added function for setting display display_number to value. (DOT IS ADDED AND MAY NEED TO BE REMOVED) */
void set_displays(int display_number, int value, int dot) {
	// the section will light up if the bit is 0
	char numbers_on_display[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90, 0xFF};

	volatile unsigned char* ptr = display_ptr + 16*display_number;
	int displaybits = numbers_on_display[value];
	if (dot) {
			displaybits -= 0x80;
	}
	*ptr = displaybits;
}

void setup_7seg_displays() {
	for (int i=0; i<6;i++) {
		set_displays(i,10,0);
	}
}

void set_score(int score) {
	int x = score % 10;
	int display = 0;
	while (score) {
		score /= 10;
		set_displays(display,x,0);
		x = score % 10;
		++display;
	}
}

void toggle_high_score_display() {
	if (*toggle_high_score) {
		set_score(*high_score);
	} else {
		setup_7seg_displays();
	}
}

void handle_interrupt(int mcause) {
	// button press
	if (mcause == 18) {
		if (*in_game) {
			jump_frames = JUMP_TIME;
		} else {
			*proceed = 1;
		}

		// button_ptr + 3 is the edge capture register 
		// (setting the bit for the button clears the edge capture)
		*(button_ptr+3) |= 0b1; 
		
	} else if (mcause == 17) {
		if (*(in_main_menu)) {
			*toggle_high_score = (*toggle_high_score + 1) % 2;
			toggle_high_score_display();
			*(switch_ptr+3) |= 0b1;
		}
	}else if (mcause == 16) {
		return;
	}
}

void labinit(void) {
	// button_ptr + 2 is the interrupt mask 
	// setting bit 1 allows button to interrupt
	*(button_ptr+2) = 0b1; 
	*(switch_ptr+2) = 0b1;
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


void drawPipes(int* pipe_idx, int* first_pipe_center_x, int VGA_offset) {
	// update first pipe if out of frame
	if (*first_pipe_center_x + PIPE_RADIUS < 0) {
		*pipe_idx = ((*pipe_idx) + 1)%1000;
		*first_pipe_center_x += PIPE_HORIZONTAL_SPACE;
	}
	int idx = *pipe_idx;
	int x_center = *first_pipe_center_x;
	
	while (x_center - PIPE_RADIUS < WIDTH) {
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

void game_over(int VGA_offset) {
	for (int i=0; i<SIZE;i++) {
		*(VGA+VGA_offset*SIZE+i) = BIRD_COLOR;
	}

	// sends vga address as output
	*(VGA_CTRL+1) = (unsigned int) VGA+VGA_offset*SIZE;
	*(VGA_CTRL+0) = 0;
	
	while (!(*proceed)) {
		//
	}
	*proceed = 0;
	while (!(*proceed)) {
		//
	}
	*proceed = 0;
}


void game() {
	*in_game = 1;
	// offset for writing to first or second half of VGA buffer
	int VGA_offset = 1;
	int bird_pos_y = BIRD_START_POS_Y;
	int pipe_idx = 0;
	int first_pipe_center_x = WIDTH + PIPE_RADIUS;
	int score = 0;
	setup_pipes();
	setup_bg_ground();
	set_displays(0,0,0);
	while (1) {
		drawBackground(VGA_offset);
		drawBird(bird_pos_y, VGA_offset);
		drawPipes(&pipe_idx, &first_pipe_center_x, VGA_offset);	


		// sends vga address as output
		*(VGA_CTRL+1) = (unsigned int) VGA+VGA_offset*SIZE;
		
		// find out what this does
		*(VGA_CTRL+0) = 0;

		// Small delay
		for (int i = 0; i < 100000; i++) {
			asm volatile("nop");
		}

		// swap side of vga buffer		
		VGA_offset = (VGA_offset+1)%2;

		if (first_pipe_center_x >= BIRD_POS_X - BIRD_RADIUS && first_pipe_center_x < BIRD_POS_X - BIRD_RADIUS + PIPE_SPEED) {
			++score;
		}

		set_score(score);

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
	*in_game = 0;
	if (score > *high_score) {
		*high_score = score;
	}
	game_over(VGA_offset);
}

void main_menu() {
	while (1) {
		*in_main_menu = 1;
		setup_7seg_displays();
		for (int i=0; i<SIZE;i++) {
			*(VGA+i) = BGR_COLOR;
		}

		volatile char* play_address = VGA+WIDTH*(HEIGHT/2-PLAY_WIDTH/2) - PLAY_WIDTH/2 +WIDTH/2;

		for (int y=0; y < 70; y++) {
			for (int x=0; x<70; x++) {
				if (2*y>x && 2*y < -x +140) {
					*(play_address + y*WIDTH + x) = 0xFF;
				}
				
			}
		}		

		// sends vga address as output
		*(VGA_CTRL+1) = (unsigned int) VGA;
		*(VGA_CTRL+0) = 0;
		
		while (!(*proceed)) {
			//
		}
		*proceed = 0;
		*in_main_menu = 0;
		*toggle_high_score = 0;		
		game();
	}
}

// for VGA 
int main(){
	labinit();
	main_menu();
}

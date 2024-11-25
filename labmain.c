volatile char *VGA = (volatile char*) 0x08000000;
volatile int *VGA_CTRL = (volatile int*) 0x04000100;

const int WIDTH = 320;
const int HEIGHT = 240;
const int SIZE = WIDTH*HEIGHT;

const char BGR_COLOR = 50;
const char GROUND_COLOR = 20;
const char BIRD_COLOR = 0b11100000;

const int BIRD_POS_X = WIDTH/5;
const int BIRD_START_POS_Y = HEIGHT/2;
const int BIRD_RADIUS = 15;

const int FALL_SPEED = 1;
const int SKY_HEIGHT = 200;

void handle_interrupt(int mcause) {

}

void testVGA() {
	// vertical scrolling
	for(int i = 0; i< 320*480; i++) {
		VGA[i] = i/320;
	}

	unsigned int y_ofs = 5;
	while(1) {
		*(VGA_CTRL+1) = (unsigned int) (VGA+y_ofs*320);
		*(VGA_CTRL+0) = 0;
		y_ofs = (y_ofs+1) % 240;
		for (int i = 0; i < 1000000; i++) {
			asm volatile ("nop");
		}

	}

}

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

		// bird is falling
		bird_pos_y += FALL_SPEED;

		// positive y means downwards
		if (bird_pos_y + BIRD_RADIUS >= SKY_HEIGHT){
			break;
		}
	}
}

// for VGA 
int main(){
	game();
}
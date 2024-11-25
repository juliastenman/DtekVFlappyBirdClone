	volatile char *VGA = (volatile char*) 0x08000000;
	volatile int *VGA_CTRL = (volatile int*) 0x04000100;

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

void testVGA2() {
	// blue skye
	for(int i = 0; i< 320*160; i++) {
		VGA[i] = 50;
	}

	// white first row ?
	for(int i = 0; i< 320; i++) {
		VGA[i] = 255;
	}


	// black
	for(int i = 320*80+100; i< 320*80+150; i++) {
		VGA[i] = 0;
	}

	// green
	for(int i = 320*160; i< 320*480; i++) {
		VGA[i] = 40;
	}
	

	unsigned int y_ofs = 0;
	unsigned int x_ofs = 0;

	while(1) {
		*(VGA_CTRL+1) = (unsigned int) (VGA+x_ofs);
		*(VGA_CTRL+0) = 0;
		x_ofs = (x_ofs+1) % 320;
		// x_ofs = (x_ofs+1) % 240;
		for (int i = 0; i < 1000000; i++) {
			asm volatile ("nop");
		}

	}
}

void testVGA3() {
	// blue skye
	for(int i = 0; i< 320*160; i++) {
		VGA[i] = 50;
	}

	// white first row ?
	for(int i = 0; i< 320; i++) {
		VGA[i] = 255;
	}


	// black
	for(int i = 320*80+100; i< 320*80+150; i++) {
		VGA[i] = 0;
	}

	// green
	for(int i = 320*160; i< 320*480; i++) {
		VGA[i] = 40;
	}
	

	unsigned int y_ofs = 0;
	unsigned int x_ofs = 0;

	while(1) {
		// *(VGA_CTRL+2) = 0x00F00140; // Resolution (YYYYXXXX)
		*(VGA_CTRL+1) = (unsigned int) (VGA+x_ofs);
		*(VGA_CTRL+0) = 0;
		x_ofs = (x_ofs+1) % 320;
		// x_ofs = (x_ofs+1) % 240;
		for (int i = 0; i < 1000000; i++) {
			asm volatile ("nop");
		}

	}
}

void testVGA4() {
	// Initialize pixel buffer
	for (int i = 0; i < 320 * 480; i++) {
		VGA[i] = (i % 320); // Horizontal gradient for visualization
	}
	// blue skye
	for(int i = 0; i< 640*160; i++) {
		VGA[i] = 50;
	}

	// white first row ?
	for(int i = 0; i< 640; i++) {
		VGA[i] = 255;
	}


	// black
	for(int i = 640*80+100; i< 640*80+150; i++) {
		VGA[i] = 0;
	}

	// green
	for(int i = 640*160; i< 640*480; i++) {
		VGA[i] = 40;
	}

	unsigned int x_ofs = 0; // Initial horizontal offset
	unsigned int y_ofs = 0; // Initial vertical offset

	while (1) {
		// Compute the base address for the VGA controller
		for (int y = 0; y < 240; y++) { // Visible screen height
			unsigned int* line_addr = (unsigned int*)(VGA + (2*y + y_ofs) * 320 + x_ofs);
			*(VGA_CTRL + y) = (unsigned int)line_addr;
		}

		// Update offsets
		x_ofs = (x_ofs + 1) % 640; // Wrap horizontally
		// y_ofs = (y_ofs + 1) % 240; // Wrap vertically (optional)

		// Small delay
		for (int i = 0; i < 1000000; i++) {
			asm volatile("nop");
		}
	}
}

// for VGA 
int main(){
	testVGA4();
	// testVGA2();
}
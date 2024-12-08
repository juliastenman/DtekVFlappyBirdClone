Note: This guide assumes the user knows how to connect the board physically and then using 
usbipd, jtag etc. as the guide will only cover the game itself.

How to play:
- Compile
    - Go to the game directory
    - Write "make" in the terminal
Start game 
    - Connect the DTEK-V board and VGA cable/converter physically
    - Run usbipd and jtag etc.
    - Execute dtekv-run in dtekv-tools with the path to your game directory 
        e.g. "./dtekv-run ../game/main.bin"
Play
    - Starting in the main menu, the user can proceed to the game by pressing button 1
    - Click button 1 to jump/fly
    - Increment your score by passing pipes without colliding
    - Your score will show on the 7 segment displays on the board
    - Don't collide with the ground or the pipes or it's game over and the screen goes red
    - Press button 1 to go back to the main menu after game over
    - Toggle the high score on the 7 segment displays by flipping switch 0
    - Press button 1 to play again 
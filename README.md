# 🐦 Flappy Bird Clone (DTEK-V Edition)

A simple Flappy Bird–style game written mostly in **C**, designed for the **DTEK-V**.

> This documentation was partially generated using AI and reviewed by the developers.
---

## 📁 Project Structure

root/
├── game/        # Game source code (this is what you run)
├── analysis/    # Supporting analysis / experiments / notes
└── README.md

---

## ⚙️ Features

- Pure C implementation (minimal dependencies)
- Dual-buffered rendering for seamless visuals
- VGA output support via DTEK-V hardware
- Button-based input controls
- 7-segment display score tracking
- High score toggle via hardware switch
- Simple state system (menu → gameplay → game over)

---

## 🛠️ Build Instructions

From the **game directory**:

cd game
make

This will generate a `main.bin` binary ready for execution on the board.

---

## ▶️ Running the Game

This guide assumes the DTEK-V board is already connected and configured (USBIPD, JTAG, etc.).

1. Physically connect:
   - DTEK-V board
   - VGA cable / converter

2. Ensure device connection via tools (e.g. usbipd, jtag)

3. Run the game using:

dtekv-run ../game/main.bin

(Adjust path if running from a different directory.)

---

## 🎮 How to Play

### Main Menu
- Press **Button 1** to start the game

### Gameplay
- Press **Button 1** to jump / fly
- Avoid pipes and the ground
- Pass pipes to increase your score
- Score is shown on the **7-segment displays**

### Game Over
- Collision with pipes or ground ends the game
- Screen turns red
- Press **Button 1** to return to the main menu

### Extras
- Toggle high score display using **Switch 0**
- Replay anytime from game over screen

---

## 🧠 Technical Notes

- Uses **double buffering** for flicker-free rendering
- Optimized for embedded constraints on DTEK-V
- No external libraries used
- Designed specifically for VGA output

---

## 🧾 Requirements

- DTEK-V board
- VGA output connection
- USBIPD + JTAG toolchain configured
- `dtekv-run` from DTEK-V tools

---

## 🔁 Loop

Learn → Flap → Fail → Repeat → Improve score

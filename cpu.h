#pragma once

class CPU {
public:
	void initialize(); //Clear the memory, registers, screen
	void emulateCycle(); //Emulate one cycle
	//void setKeys(); //Store key press state (Press and Release)
	void loadGame(unsigned char*, long); //Copy program into memory
	//void drawGraphics();

	unsigned char drawFlag;
	unsigned char gfx[64 * 32]; //byte array to keep track of pixels (1 = on, 0 = off)
	unsigned char key[16]; //byte array to store key state

	//SUPER-CHIP8
	unsigned short screenWidth;
	unsigned short screenHeight;

private:
	//Variables
	unsigned short opcode; //2 byte variable used to store current opcode
	unsigned char memory[4096]; //byte array to simulate 4K of memory
	unsigned char V[16]; //byte array to simulate 16 8-bit CPU registers
	unsigned short I; //2 byte index register
	unsigned short pc; //program counter (0x000 - 0xFFF)

	unsigned char delay_timer; //60Hz count register, counts to zero if set above 0
	unsigned char sound_timer; //60Hz count register, counts to zero if set above 0, system's buzzer sounds when reaching 0

	unsigned short stack[16]; //stack levels
	unsigned short sp; //stack pointer

	//SUPER-CHIP8
	unsigned char rpl[7];
	unsigned char exitInterpreter;
	unsigned char sMode;

	unsigned char chip8_fontset[80] =
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	 ///	System Memory Map
	 ///	0x000 - 0x1FF - Chip 8 interpreter(contains font set in emu)
	 ///	0x050 - 0x0A0 - Used for the built in 4x5 pixel font set(0 - F)
	 ///	0x200 - 0xFFF - Program ROM and work RAM
};
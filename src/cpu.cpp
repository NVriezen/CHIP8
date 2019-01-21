#include "cpu.h"
#include <stdio.h>
#include <cstdlib>
#include <time.h>

#include <iostream>

void CPU::initialize() {
	pc = 0x200;
	opcode = 0;
	I = 0;
	sp = 0;

	delay_timer = 0;
	sound_timer = 0;

	srand(time(NULL)); //Set seed for random function
	drawFlag = true; //

	///SUPER-CHIP8
	sMode = false;
	screenHeight = 32;
	screenWidth = 64;
	//gfx = new unsigned char[screenHeight * screenWidth];

	//clearing memory
	for (auto& m : memory) {
		m = 0;
	}

	//resetting registers
	for (auto& v : V) { 
		v = 0;
	}

	//clearing stack
	for (auto& s : stack) {
		s = 0;
	}

	//clearing display
	for (auto& d : gfx) {
		d = 0;
	}

	///SUPER-CHIP8: Reset RPL user flags
	for (auto& r : rpl) {
		r = 0;
	}

	//write fontset to memory
	for (size_t i = 0; i < 80; i++) {
		memory[i] = chip8_fontset[i];
	}
}

void CPU::emulateCycle() {
	//Fetch Opcode
	opcode = memory[pc] << 8 | memory[pc + 1];
	//printf("opcode: " +opcode);
	//printf("PC: ");// +pc);
	//printf("I: ");// +I);
	std::cout << "[opcode - " << opcode; //<< std::endl;
	std::cout << "] - [pc - " << pc; // << std::endl;
	std::cout << "] [I - " << I; // << std::endl;
	std::cout << "] [sp - " << sp; // << std::endl;
	std::cout << "] [stack " << stack[sp] << "]"<< std::endl;

	//Decode Opcode
	switch (opcode & 0xF000) {
		//----------------------------------------------------------------
		case 0x0000: 
		{	
			switch (opcode & 0x00F0) {
				case 0x0010: //0x001N: 
				{
					pc += 2;
				}
				break;
				case 0x00B0: //0x00BN: 
				{
					pc += 2;
				}
				break;
				case 0x00C0: //0x00CN: 
				{
					pc += 2;
				}
				break;
				case 0x00E0:
				{
					switch (opcode & 0x000F) {
						case 0x0000: //0x00E0: Clears the screen
						{
							/*/
							if (sMode) {
								unsigned char display[128 * 64];
								display = gfxSuper;
							}
							else {
								unsigned char display[32 * 64];
								display = gfx;
							}
							*/
							for (auto& d : gfx) {
								d = 0;
							}
							//for (int i = 0; i < 2048; ++i) {
							//	gfx[i] = 0x0;
							//}
							drawFlag = true;
							pc += 2;
						}
						break;
						case 0x000E: //0x00EE: Returns from subroutine 
						{
							--sp;
							pc = stack[sp];
							//--sp;
							pc += 2;
						}
						break;
						default:
							printf("Unknown opcode: 0x%X\n", opcode);
						break;
					}
				}
				break;
				case 0x00F0:
				{
					switch (opcode & 0x000F) {
						case 0x000A: //0x00FA: 
						{
							pc += 2;
						}
						break;
						case 0x000B: //0x00FB: Scroll display 4 pixels to the right
						{
							for (size_t i = 0; i < (screenHeight * screenWidth); i++) {

							}
							pc += 2;
						}
						break;
						//case 0x000C: //0x00FC:
						//{
						//	pc = stack[--sp];
						//	pc += 2;
						//}
						break;
						case 0x000D: //0x00FD: Exit the interpreter
						{
							exitInterpreter = true;
						}
						break;
						//case 0x000E: //0x00FE: Enable low res (64x32) mode
						//{
							//sMode = false;
							//screenHeight = 32;
							//screenWidth = 64;
							//delete gfx;
							//gfx = new unsigned char[screenHeight * screenWidth];
							//pc += 2;
						//}
						//break;
						case 0x000F: //0x00FF: Enable high res (128x64) mode
						{
							sMode = true;
							screenHeight = 64;
							screenWidth = 128;
							//gfx = new unsigned char[screenHeight * screenWidth];
							pc += 2;
						}
						break;
						default:
							printf("Unknown opcode: 0x%X\n", opcode);
							exit(1);
						break;
					}
				}
				break;
				//case 0x00A0: //0x02A0: 
				//{

				//}
				//break;
				default:
					printf("Unknown opcode: 0x%X\n", opcode);
				break;
			}
		}
		break;
		//----------------------------------------------------------------
		case 0x1000: //0x1NNN: Jump to location nnn.
			pc = opcode & 0x0FFF;
		break;
		//----------------------------------------------------------------
		case 0x2000: //0x2NNN: Calls subroutine at NNN.
		{
			stack[sp] = pc;
			++sp;
			pc = opcode & 0x0FFF;
		}
		break;
		//----------------------------------------------------------------
		case 0x3000: //0x3XNN: Skips the next instruction if VX equals NN.
		{
			if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) {
				pc += 2;
			}
			pc += 2;
		}
		break;
		//----------------------------------------------------------------
		case 0x4000: //0x4XNN: Skips the next instruction if VX doesn't equal NN.
		{
			if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) {
				pc += 2;
			}
			pc += 2;
		}
		break;
		//----------------------------------------------------------------
		case 0x5000: //0x5XY0: Skips the next instruction if VX equals VY. 
		{
			if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) {
				pc += 2;
			}
			pc += 2;
		}
		break;
		//----------------------------------------------------------------
		case 0x6000: //0x6XNN: Sets VX to NN.
		{
			V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
			pc += 2;
		}
		break;
		//----------------------------------------------------------------
		case 0x7000: //0x7XNN: Adds NN to VX.
		{
			V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
			pc += 2;
		}
		break;
		//----------------------------------------------------------------
		case 0x8000:
		{
			switch (opcode & 0x000F) {
				case 0x0000: //0x8XY0: Sets VX to the value of VY.
				{
					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
					pc += 2;
				}
				break;
				case 0x0001: //0x8XY1: Sets VX to VX OR VY.
				{
					V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
					pc += 2;
				}
				break;
				case 0x0002: //0x8XY2: Sets VX to VX AND VY.
				{
					V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
					pc += 2;
				}
				break;
				case 0x0003: //0x8XY3: Sets VX to VX XOR VY.
				{
					V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
					pc += 2;
				}
				break;
				case 0x0004: //0x8XY4: Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
				{
					if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8])) {
						V[0xF] = 1; //carry
					}
					else {
						V[0xF] = 0; //no carry
					}
					V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
					pc += 2;
				}
				break;
				case 0x0005: //0x8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
				{
					if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8]) {
						V[0xF] = 0;
					}
					else {
						V[0xF] = 1;
					}
					V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
					pc += 2;
				}
				break;
				case 0x0006: ////0x8XY6: Stores the least significant bit of VX in VF and then shifts VX to the right by 1.
				{
					V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
					V[(opcode & 0x0F00) >> 8] >>= 1;
					pc += 2;
				}
				break;
				case 0x0007: //0x8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
				{
					if (V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4]) {
						V[0xF] = 0;
					}
					else {
						V[0xF] = 1;
					}
					V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
					pc += 2;
				}
				break;
				case 0x000E: //0x8XYE: Stores the most significant bit of VX in VF and then shifts VX to the left by 1.
				{
					V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
					V[(opcode & 0x0F00) >> 8] <<= 1;
					pc += 2;
				}
				break;
				default:
					printf("Unknown opcode: 0x%X\n", opcode);
				break;
			}
		}
		break;
		//----------------------------------------------------------------
		case 0x9000: //0x9XY0: Skips the next instruction if VX doesn't equal VY. 
		{
			if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4]) {
				pc += 2;
			}
			pc += 2;
		}
		break;
		//----------------------------------------------------------------
		case 0xA000: //ANNN: Sets I to the address NNN.
		{
			I = opcode & 0x0FFF;
			pc += 2;
		}
		break;
		//----------------------------------------------------------------
		case 0xB000: //0xBNNN: Jumps to the address NNN plus V0.
			pc = (opcode & 0x0FFF) + V[0];
		break;
		//----------------------------------------------------------------
		case 0xC000: //0xCXNN: Sets VX to the result of a bitwise AND operation on a random number and NN.
		{
			//unsigned char randomNum = rand() % 0xFF;
			V[(opcode & 0x0F00) >> 8] = (rand() % 0xFF) & (opcode & 0x00FF);
			pc += 2;
		}
		break;
		//----------------------------------------------------------------
		case 0xD000:
		{
			unsigned short x = V[(opcode & 0x0F00) >> 8];
			unsigned short y = V[(opcode & 0x00F0) >> 4];
			unsigned short spritePixel;
			/*
			if ((opcode & 0x000F) == 0 && sMode) { //0xDXY0: Draws a 16x16 sprite at coordinate (VX, VY).
				V[0xF] = 0;
				for (size_t row = 0; row < 16; row++) { //loop through row of sprite
					spritePixel = memory[I + row]; //Set 8 bits pixel line;
					for (size_t column = 0; column < 16; column++) { //Loop through column of sprite
						if ((spritePixel & (0x80 >> column)) != 0) { //Loop through byte, one bit at a time
							if (gfx[x + column + ((y + row) * screenWidth)] == 1) {
								V[0xF] = 1;
							}
							gfx[x + column + ((y + row) * screenWidth)] ^= 1;
						}
					}
				}
			}
			else { //0xDXYN: Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels.
			*/	
				//unsigned short x = V[(opcode & 0x0F00) >> 8];
				//unsigned short y = V[(opcode & 0x00F0) >> 4];
				unsigned short height = opcode & 0x000F;
				//unsigned short spritePixel;

				V[0xF] = 0;
				for (size_t row = 0; row < height; row++) { //loop through row of sprite
					spritePixel = memory[I + row]; //Set 8 bits pixel line;
					for (size_t column = 0; column < 8; column++) { //Loop through column of sprite
						if ((spritePixel & (0x80 >> column)) != 0) { //Loop through byte, one bit at a time
							if (gfx[x + column + ((y + row) * screenWidth)] == 1) {
								V[0xF] = 1;
							}
							gfx[x + column + ((y + row) * screenWidth)] ^= 1;
						}
					}
				}
			//}

			drawFlag = true;
			pc += 2;
		}
		break;
		//----------------------------------------------------------------
		case 0xE000:
		{
			switch (opcode & 0x00FF) {
				case 0x009E: //0xEX9E: Skips the next instruction if the key stored in VX is pressed.
				{
					if (key[V[(opcode & 0x0F00) >> 8]] != 0) {
						pc += 2;
					}
					pc += 2;
				}
				break;
				case 0x00A1: //0xEXA1: Skips the next instruction if the key stored in VX isn't pressed.
				{
					if (key[V[(opcode & 0x0F00) >> 8]] == 0) {
						pc += 2;
					}
					pc += 2;
				}
				break;
				default:
					printf("Unknown opcode: 0x%X\n", opcode);
				break;
			}
		}
		break;
		//----------------------------------------------------------------
		case 0xF000:
		{
			switch (opcode & 0x00FF) {
				case 0x0007: //0xFX07: Sets VX to the value of the delay timer.
					V[(opcode & 0x0F00) >> 8] = delay_timer;
					pc += 2;
				break;
				case 0x000A: //0xFX0A: A key press is awaited, and then stored in VX. 
					//stop execution until key press
				{
					bool keyReceived = false;

					for (size_t i = 0; i < 16; i++) {
						if (key[i] != 0) {
							V[(opcode & 0x0F00) >> 8] = i;
							keyReceived = true;
						}
					}

					if (!keyReceived) {
						return;
					}
					pc += 2; //put this in while loop to wait?
				}
				break;
				case 0x0015: //0xFX15: Sets the delay timer to VX.
				{
					delay_timer = V[(opcode & 0x0F00) >> 8];
					pc += 2;
				}
				break;
				case 0x0018: //0xFX18: Sets the sound timer to VX.
				{
					sound_timer = V[(opcode & 0x0F00) >> 8];
					pc += 2;
				}
				break;
				case 0x001E: //0xFX1E: Adds VX to I.
				{
					if (I + V[(opcode & 0x0F00) >> 8] > 0xFFF)	// VF is set to 1 when range overflow (I+VX>0xFFF), and 0 when there isn't.
						V[0xF] = 1;
					else
						V[0xF] = 0;
					I += V[(opcode & 0x0F00) >> 8];
					pc += 2;
				}
				break;
				case 0x0029: //0xFX29: Sets I to the location of the sprite for the character in VX.
				{
					I = (V[(opcode & 0x0F00) >> 8] * 0x5);
					pc += 2;
				}
				break;
				case 0x0030: //0xFX30: Set I to the address of the SCHIP-8 16x10 font sprite representing the value in VX.
				{
					I = (V[(opcode & 0x0F00) >> 8] * 10);
					pc += 2;
				}
				break;
				case 0x0033: //0xFX33: Store BCD representation of Vx in memory locations I, I+1, and I+2.
				{
					memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
					memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
					memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
					pc += 2;
				}
				break;
				case 0x0055: //0xFX55: Stores V0 to VX (including VX) in memory starting at address I.
				{
					for (size_t i = 0; i <= ((opcode & 0x0F00) >> 8); i++) {
						memory[I + i] = V[i];
					}
					I += ((opcode & 0x0F00) >> 8) + 1; // On the original interpreter, when the operation is done, I = I + X + 1.
					pc += 2;
				}
				break;
				case 0x0065: //0xFX65: Fills V0 to VX (including VX) with values from memory starting at address I.
				{
					for (size_t i = 0; i <= ((opcode & 0x0F00) >> 8); i++) {
						V[i] = memory[I + i];
					}
					I += ((opcode & 0x0F00) >> 8) + 1; // On the original interpreter, when the operation is done, I = I + X + 1.
					pc += 2;
				}
				break;
				case 0x0075: //0xFX75: Store V0 through VX to HP-48 RPL user flags (X <= 7)
				{
					if ((opcode & 0x0F00 >> 8) <= 7) {
						for (size_t i = 0; i <= ((opcode & 0x0F00) >> 8); i++) {
							rpl[i] = V[i];
						}
					}
					pc += 2;
				}
				break;
				case 0x0085: //0xFX85: Read V0 through VX to HP-48 RPL user flags (X <= 7)
				{
					if ((opcode & 0x0F00 >> 8) <= 7) {
						for (size_t i = 0; i <= ((opcode & 0x0F00) >> 8); i++) {
							V[i] = rpl[i];
						}
					}
					pc += 2;
				}
				break;
				default:
					printf("Unknown opcode: 0x%X\n", opcode);
				break;
			}
		}
		break;
		//----------------------------------------------------------------
		default:
			printf("Unknown opcode: 0x%X\n", opcode);
		break;
	}

	//Update timers
	if (delay_timer > 0) {
		--delay_timer;
	}
	if (sound_timer > 0) {
		if (sound_timer == 1) {
			printf("\a"); //bleep
		}
		--sound_timer;
	}
}

void CPU::loadGame(unsigned char* rom, long romSize) {
	if (rom != NULL || romSize != NULL) {
		for (size_t i = 0; i < romSize; i++) {
			memory[i + 512] = rom[i];
		}
	} else {
		printf("Can't find file");
		return;
	}
}

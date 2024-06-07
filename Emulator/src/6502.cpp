#include <stdio.h>
#include <stdlib.h>

#include "6502.h"

namespace E6502 {
	// https://www.c64-wiki.com/wiki/Reset_(Process)
	void CPU::Reset(Memory& memory) {
		PC = Memory::ResetVector;
		SP = Memory::StackEnd;
		A = X = Y = 0;
		// reset all flags
		S_N = S_V = S_E = S_B = 0;
		S_D = S_I = S_Z = S_C = 0;
		memory.Initilize();
	}

	Byte CPU::Fetch(Memory& memory, DWord& cycles) {
		Byte data = memory.Read(PC);
		PC++;
		cycles--;
		return data;
	}
	DWord CPU::FetchWord(Memory& memory, DWord& cycles) {
		DWord data = memory.ReadWord(PC);
		PC+=2;
		cycles-=2;
		return data;
	}

	Byte CPU::Read(Memory& memory, DWord Address, DWord& cycles) {
		Byte data = memory.Read(Address);
		cycles--;
		return data;
	}
	DWord CPU::ReadWord(Memory& memory, DWord address, DWord& cycles) {
		DWord word = memory.ReadWord(address);
		cycles -= 2;
		return word;
	}


	void CPU::Write(Memory& memory, Byte byte, DWord address, DWord& cycles) {
		memory.Write(address, byte);
		cycles -= 1;
	}
	void CPU::WriteWord(Memory& memory, DWord word, DWord address, DWord& cycles) {
		memory.WriteWord(address, word);
		cycles -= 2;
	}

	Byte CPU::PopStack(Memory& memory, DWord& cycles) {
		Byte byte = Read(memory, SP, cycles);
		SP += 1;
		cycles--;
		return byte;
	}
	DWord CPU::PopStackWord(Memory& memory, DWord& cycles) {
		DWord word = ReadWord(memory, SP, cycles);
		SP += 2;
		cycles--;
		return word;
	}

	void CPU::PushStack(Memory& memory, Byte byte, DWord& cycles) {
		cycles -= 1;
		SP -= 1;
		Write(memory, byte, SP, cycles);
	}
	void CPU::PushStackWord(Memory& memory, DWord word, DWord& cycles) {
		SP -= 2;
		cycles--;
		WriteWord(memory, word, SP, cycles);
	}

	void CPU::ZeroFlag() {
		S_Z = (A == 0);
	}
	void CPU::NegativeFlag() {
		S_N = (A & 0b10000000) > 0;
	}
	void CPU::LDAStatus() {
		ZeroFlag();
		NegativeFlag();
	}

	// https://web.archive.org/web/20210803072316/http://www.obelisk.me.uk/6502/reference.html
	void CPU::Execute(Memory& memory, DWord cycles) {
		while (cycles > 0) {
			cycles = ExecuteOne(memory, cycles);
		}
	}

	DWord CPU::ExecuteOne(Memory& memory, DWord cycles) {
		// current instruction
		Byte current = Fetch(memory, cycles);

		switch ((Instruction)current) {
		case Instruction::BRK: {
			PushStackWord(memory, PC, cycles);
			PushStack(memory, S_N, cycles);
			PC = Memory::InterruptVector;
			cycles--;
			S_B = 1;
			break;
		}
		case Instruction::JSR: {
			DWord address = FetchWord(memory, cycles);
			PushStackWord(memory, PC - 1, cycles);
			PC = address;
			break;
		}
		case Instruction::JMP_ABS: {
			DWord address = FetchWord(memory, cycles);
			PC = address;
			break;
		}
		case Instruction::RTS: {
			DWord address = PopStackWord(memory, cycles) + 1;
			PC = address;
			cycles -= 2;
			break;
		}
		case Instruction::PHA: {
			PushStack(memory, A, cycles);
			break;
		}
		case Instruction::PLA: {
			A = PopStack(memory, cycles);
			cycles -= 1;
			ZeroFlag();
			break;
		}
		case Instruction::LDA_IM: {
			Byte value = Fetch(memory, cycles);
			A = value;
			LDAStatus();
			break;
		}
		case Instruction::LDA_ZP: {
			Byte ZPA = Fetch(memory, cycles);
			A = Read(memory, ZPA, cycles);
			LDAStatus();
			break;
		}
		case Instruction::LDA_ABS: {
			DWord ZPA = FetchWord(memory, cycles);
			A = Read(memory, ZPA, cycles);
			LDAStatus();
			break;
		}
		case Instruction::STA_ZP: {
			Byte ZPA = Fetch(memory, cycles);
			Write(memory, (Byte)A, (DWord)ZPA, cycles);
			break;
		}
		case Instruction::STA_ABS: {
			DWord address = FetchWord(memory, cycles);
			Write(memory, (Byte)A, address, cycles);
			break;
		}
		}
		return cycles;
	}

	void CPU::Dump(const char* fName) const {
		if (FILE* file = fopen(fName, "wb+")) {
			
			fprintf(file, "PC: 0x%02hhx\n", PC);
			fprintf(file, "SP: 0x%02hhx\n", SP);
			fprintf(file, "\n");
			fprintf(file, "A: 0x%02hhx\n", A);
			fprintf(file, "Y: 0x%02hhx\n", X);
			fprintf(file, "X: 0x%02hhx\n", Y);
			fprintf(file, "\n");
			fprintf(file, "Negative flag: 0x%02hhx\n", S_N & 0x01);
			fprintf(file, "Overflow flag: 0x%02hhx\n", S_V & 0x02);
			fprintf(file, "Brea commandk: 0x%02hhx\n", S_B & 0x04);
			fprintf(file, "Decimal mode: 0x%02hhx\n", S_D & 0x05);
			fprintf(file, "Interrupt disable: 0x%02hhx\n", S_I & 0x06);
			fprintf(file, "Zero flag: 0x%02hhx\n", S_Z & 0x07);
			fprintf(file, "Carry flag: 0x%02hhx\n", S_C & 0x08);

			fclose(file);
		}
	}
}
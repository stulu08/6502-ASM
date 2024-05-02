#pragma once
#include "Memory.h"

namespace E6502 {
	// https://en.wikipedia.org/wiki/MOS_Technology_6502
	class CPU {
	public:
		// Program counter
		DWord PC;
		// Stack pointer
		DWord SP;
		// Registers
		Byte A; // accumulator
		Byte X, Y; // x,y index

		// Processor Status
		Byte S_N : 1; // negative flag
		Byte S_V : 1; // overflow flag
		Byte S_E : 1; // empty
		Byte S_B : 1; // break command
		Byte S_D : 1; // decimal mode
		Byte S_I : 1; // interrupt disable
		Byte S_Z : 1; // zero flag
		Byte S_C : 1; // carry flag

		void (*interruptCallback)(Byte) = NULL;


		void Reset(Memory& memory);
		void Execute(Memory& memory, DWord cycles);
		DWord ExecuteOne(Memory& memory, DWord cycles = UINT16_MAX - 1);

		Byte Fetch(Memory& memory, DWord& cycles);
		DWord FetchWord(Memory& memory, DWord& cycles);

		Byte Read(Memory& memory, DWord Address, DWord& cycles);
		DWord ReadWord(Memory& memory, DWord address, DWord& cycles);

		void Write(Memory& memory, Byte byte, DWord address, DWord& cycles);
		void WriteWord(Memory& memory, DWord word, DWord address, DWord& cycles);

		Byte PopStack(Memory& memory, DWord& cycles);
		DWord PopStackWord(Memory& memory, DWord& cycles);

		void PushStack(Memory& memory, Byte byte, DWord& cycles);
		void PushStackWord(Memory& memory, DWord word, DWord& cycles);

		void LDAStatus();
		
		void Dump(const char* fName) const;
	};
}
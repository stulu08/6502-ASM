#pragma once
#include "Config.h"
#include <stdio.h>

namespace E6502 {
	using Byte = uint8_t;
	using DWord = uint16_t;
	enum class Instruction : Byte {
		BRK = 0x00,

		JSR = 0x20,
		JMP_ABS = 0x4C,
		RTS = 0x60,

		PHA = 0x48,
		PLA = 0x68,

		LDA_IM = 0xA9,
		LDA_ZP = 0xA5,
		LDA_ZPX = 0xB5,
		LDA_ABS = 0xAD,

		STA_ZP = 0x85,
		STA_ABS = 0x8D,

		ADC_IM = 0x69,
		ADC_ZP = 0x69,
		ADC_ZPX = 0x69,
		ADC_ABS = 0x69,
		ADC_ABSX = 0x69,
		ADC_ABSY = 0x69,

	};
	class Memory {
	public:
		// https://www.ele.uva.es/~jesus/6502copy/proto.html
		static inline constexpr DWord Size = E6502_MEM_SIZE;
		static inline constexpr DWord ZeroPage = 0x0000;
		static inline constexpr DWord Stack = 0x0100;
		static inline constexpr DWord StackEnd = 0x0200;
		static inline constexpr DWord IOPeripherals = 0x0200;
		static inline constexpr DWord Free = 0x0300;
		static inline constexpr DWord ROM = 0xE000;
		static inline constexpr DWord ResetVector = 0xFFFC;
		static inline constexpr DWord InterruptVector = 0xFFFE;

		void Initilize() {
			if (Data != NULL) {
				for (DWord i = 0; i < Size; i++) {
					Data[i] = 0;
				}
			}
		}
		void Load(const char* fName) {
			if (FILE* file = fopen(fName, "rb")) {
				fread(Data, sizeof(Byte), Size, file);
				fclose(file);
			}
		}

		void Dump(const char* fName) const {
			if (FILE* file = fopen(fName, "wb+")) {
				fwrite(Data, sizeof(Byte), Size, file);
				fclose(file);
			}
		}



		inline void WriteWord(DWord address, DWord word) {
			Byte* bytes = (Byte*)&word;
			Data[address] = bytes[0];
			Data[address + 1] = bytes[1];
			if (writeCallback)
				(*writeCallback)(address);
		}
		inline DWord ReadWord(DWord address) const {
			Byte bytes[2];
			bytes[0] = Data[address];
			bytes[1] = Data[address+1];
			return *(DWord*)(bytes);
		}
		
		inline void Write(DWord address, Byte byte) {
			Data[address] = byte;
			if(writeCallback)
				(*writeCallback)(address);
		}
		inline Byte Read(DWord address) const {
			Byte byte = Data[address];
			return byte;
		}

		void (*writeCallback)(DWord) = NULL;

	private:
		Byte Data[Size];
	};
}